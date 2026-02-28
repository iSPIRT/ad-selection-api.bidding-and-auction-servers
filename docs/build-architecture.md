# Build Architecture & Vulnerability Upgrade Reference

## Repo Structure

```
.                                       # Root Bazel workspace (main services)
├── .bazelversion                       # Bazel version for main build (via bazelisk)
├── .bazelrc                            # Bazel flags for main build
├── WORKSPACE                           # External deps: rules_docker, grpc, abseil, etc.
├── third_party/
│   └── container_deps.bzl              # Runtime container base images (distroless, envoy)
├── services/
│   ├── buyer_frontend_service/
│   ├── bidding_service/
│   ├── seller_frontend_service/
│   ├── auction_service/
│   └── inference_sidecar/              # Separate Bazel workspaces (independent builds)
│       ├── common/                     # Shared inference lib; own WORKSPACE/.bazelversion/.bazelrc
│       └── modules/
│           ├── tensorflow_v2_14_0/     # TF sidecar; own WORKSPACE/.bazelversion/.bazelrc
│           └── pytorch_v2_1_1/         # PyTorch sidecar; own WORKSPACE/.bazelversion/.bazelrc
├── production/packaging/
│   ├── build_and_test_all_in_docker    # Top-level build orchestrator script
│   ├── azure/                          # Azure-specific packaging
│   ├── aws/                            # AWS-specific packaging (AMI)
│   └── gcp/                            # GCP-specific packaging
└── builders/
    ├── tools/
    │   ├── cbuild                      # Docker container runner (mounts workspace + cache)
    │   └── bazel-debian                # Wrapper: runs bazel inside build-debian container
    └── images/
        └── build-debian/               # Dockerfile for the build container image
```

**Key point:** The repo has **4 independent Bazel workspaces** (root + 3 inference sidecars), each with their own `WORKSPACE`, `.bazelversion`, and `.bazelrc`. They share a host-mounted Bazel cache but use separate output user roots.

## Build Process Layers

A single build command orchestrates **5 distinct layers**, each introducing its own dependency surface:

```
┌─────────────────────────────────────────────────────────┐
│  Layer 1: Host VM                                       │
│  └─ bazelisk, Docker daemon                             │
│  └─ Bazel cache at ~/.cache/bazel (persists across      │
│     builds, volume-mounted into containers)             │
├─────────────────────────────────────────────────────────┤
│  Layer 2: Build Container (build-debian Docker image)   │
│  └─ Clang, Python, system libs for compilation          │
│  └─ /bazel_root mounted from host ~/.cache/bazel        │
│  └─ /src/workspace mounted from host repo               │
├─────────────────────────────────────────────────────────┤
│  Layer 3: Bazel (controlled by .bazelversion)           │
│  └─ Embedded JDK (Bazel 6→JDK 11, Bazel 7→JDK 21)     │
│  └─ Remote JDK toolchain downloads                      │
│  └─ All artifacts land in host cache via volume mount   │
├─────────────────────────────────────────────────────────┤
│  Layer 4: WORKSPACE Dependencies                        │
│  └─ rules_docker, grpc, protobuf, abseil, boringssl     │
│  └─ Transitive deps (rules_apple, upb, etc.)            │
│  └─ TF/PyTorch + their own transitive dep trees         │
├─────────────────────────────────────────────────────────┤
│  Layer 5: Runtime Container Images                      │
│  └─ Base image: gcr.io/distroless/cc-debian{11,12}      │
│  └─ Envoy sidecar: envoyproxy/envoy-distroless:vX.Y.Z  │
│  └─ Flattened into final Docker image via rules_docker  │
│  └─ These determine glibc/openssl in shipped images     │
└─────────────────────────────────────────────────────────┘
```

## Build Execution Flow

```
build_and_test_all_in_docker
│
├── Phase 1: Inference Sidecars (if not --skip-inference)
│   ├── cd tensorflow_v2_14_0/ → builders/tools/bazel-debian run //:generate_artifacts
│   │   └─ cbuild → Docker(build-debian) → Bazel {TF's .bazelversion} → binary artifact
│   └── cd pytorch_v2_1_1/    → builders/tools/bazel-debian run //:generate_artifacts
│       └─ cbuild → Docker(build-debian) → Bazel {PyTorch's .bazelversion} → binary artifact
│
├── Phase 2: Main Build
│   └── builder::cbuild_debian → Docker(build-debian) → Bazel {root .bazelversion}
│       ├── bazel build //production/packaging/{platform}/{service}/...
│       └── bazel run .../copy_to_dist  →  dist/{service}_image.tar
│
└── Phase 3: Platform Packaging (azure/aws/gcp-specific)
```

**Critical detail:** Each inference sidecar is an independent Bazel workspace. `cbuild` volume-mounts `~/.cache/bazel` to `/bazel_root` inside Docker. Different Bazel versions create separate install bases under this shared cache, each embedding their own JDK.

## Vulnerability Surface Map

| Vuln Surface | Controlled By | Affects |
|---|---|---|
| glibc, libssl in shipped images | `third_party/container_deps.bzl` (distroless + envoy image digests) | CVE scans of Docker images |
| Embedded JDK in Bazel | `.bazelversion` (×4 workspaces) | VAPT scans of build VM |
| Remote JDK toolchain | `.bazelrc` `--java_runtime_version` | VAPT scans of build VM |
| OpenSSL in build cache | Bazel's BoringSSL dep (WORKSPACE) | VAPT scans of build VM |
| Build container OS packages | `builders/images/build-debian/Dockerfile` | VAPT scans of build VM |
| Transitive deps (grpc, protobuf, etc.) | WORKSPACE → `tf_workspace{0,1,2,3}()`, `grpc_deps()`, `data_plane_shared_deps_*()` | Build-time supply chain |

## Upgrade Protocol

### Step 0: Identify the Layer

Map each CVE to its layer using the table above. Don't shotgun — a glibc CVE in the Docker image is Layer 5, not Layer 3.

### Step 1: Runtime Image Vulnerabilities (Layer 5)

**File:** `third_party/container_deps.bzl`

1. Identify which base image carries the vuln (`distroless/cc-debian*` or `envoy-distroless`)
2. Find updated image digests (from GCR or Docker Hub; match `amd64`/`arm64` architecture digests)
3. Update the `digest` values. If switching Debian generations (e.g., 11→12), update the `registry`/`repository` fields too
4. **Watch for:** Envoy image is flattened on top of the distroless base. If both carry conflicting libs, the envoy layer wins. Always upgrade both together

### Step 2: Bazel / JDK Vulnerabilities (Layer 3)

**Files:** `.bazelversion` (×4), `.bazelrc` (×4), `WORKSPACE` (×4)

1. Update `.bazelversion` in **all 4 workspaces** (root + 3 inference sidecars)
2. Add compatibility flags to each `.bazelrc`:
   - Major version jumps (e.g., 6→7) typically need: `common --noenable_bzlmod`, `build --features=-parse_headers`
   - JDK pinning: `build --java_runtime_version=remotejdk_NN`, `build --tool_java_runtime_version=remotejdk_NN`
3. Update `rules_docker` in the root `WORKSPACE` if needed for Bazel compat
4. **Watch for:** Inference sidecars have deep transitive dep trees (especially TF). Check the next section

### Step 3: Transitive Dependency Conflicts (Layer 4)

**The `tf_http_archive` override pattern:** TensorFlow's workspace macros pin old versions of many deps. Because `tf_http_archive` checks `native.existing_rule(name)` and skips if already defined, you can **override any TF-pinned dep by defining it earlier in the WORKSPACE**, before calling `tf_workspace{N}()`.

Protocol:
1. Build fails with error in `external/{dep_name}/...`
2. Find what version TF pins: grep for `name = "{dep_name}"` in the cached `external/org_tensorflow/tensorflow/workspace{0,1,2,3}.bzl`
3. Find what version the dep's own consumers want: check `grpc_deps.bzl`, `grpc_extra_deps.bzl`, etc.
4. Add an `http_archive` for `{dep_name}` at the newer version **before** the `tf_workspace{N}()` call that originally defined it
5. Verify the newer version's API is backward-compatible for TF's usage (for iOS/Apple deps on Linux builds, this is almost always safe since those rules are loaded but never executed)

**Example (from this repo):** TF 2.14.0 pins `rules_apple` 2.3.0 in `tf_workspace2()`. gRPC's `grpc_deps()` wants 3.1.1. Fix: define `rules_apple` 3.1.1 before `tf_workspace2()`.

### Step 4: Build Container Vulnerabilities (Layer 2)

**Files:** `builders/images/build-debian/Dockerfile`, `builders/images/build-debian/install_apps`

These affect the build VM's VAPT scan but not shipped images. Update the Dockerfile's base image or pinned package versions.

### Step 5: Verify

After any change:

```bash
# Full build (long but definitive):
./production/packaging/build_and_test_all_in_docker \
  --service-path buyer_frontend_service --service-path bidding_service \
  --no-precommit --no-tests --build-flavor prod --platform azure --instance local

# Verify runtime images:
docker run --rm --entrypoint="" bazel/.../server_docker_image \
  sh -c '/lib/x86_64-linux-gnu/libc.so.6 2>&1 | head -1'    # glibc version
docker run --rm --entrypoint="" bazel/.../server_docker_image \
  sh -c 'ls /usr/lib/x86_64-linux-gnu/libssl*'               # OpenSSL version
docker run --rm --entrypoint="" bazel/.../server_docker_image \
  cat /etc/os-release                                          # Debian version

# Verify build VM cache:
for d in ~/.cache/bazel/*/install/*/; do                       # JDK in install bases
  "${d}embedded_tools/jdk/bin/java" -version 2>&1 | head -1
done
find ~/.cache/bazel -name "java" -executable -type f \         # Any JDK 11 anywhere
  -exec sh -c '"{}" -version 2>&1 | grep -q "\"11\."' \; -print
```

### Gotchas & Lessons Learned

1. **Shared Bazel cache is a VAPT liability.** All 4 workspaces share `~/.cache/bazel` via volume mount. A single workspace using an old Bazel version leaves its JDK in the shared cache even after the build finishes.

2. **Envoy layer can mask distroless upgrades.** The Docker image is built by flattening layers. If envoy-distroless is based on Debian 11 but the base is Debian 12, the envoy layer's older libs (including its `/etc/os-release`) take precedence. Always upgrade both images in `container_deps.bzl` together.

3. **`tf_http_archive` has `maybe` semantics.** First definition wins. This is both a risk (TF pins old deps before others can) and an opportunity (you can override by defining deps earlier).

4. **Inference sidecar builds are independent.** They have their own WORKSPACE, .bazelversion, .bazelrc, and build container invocations. Changes to the root workspace don't affect them and vice versa. Always check all 4 workspaces.

5. **`--skip-inference` / `--skip-rebuild-inference-sidecar` flags** exist for faster iteration when only changing the main build. Use them, but remember to do a full build (without these flags) for final verification.

6. **Bazel major version upgrades** often break Starlark APIs. The pattern is: upgrade → build → read the error → trace the dep chain → override or patch the offending dep. The errors are always in `external/` cached repos, never in your source code.
