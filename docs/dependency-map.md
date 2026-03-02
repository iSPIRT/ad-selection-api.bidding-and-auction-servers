# Dependency Map & Coupling Analysis

> **Purpose:** Comprehensive reference for understanding what depends on what, where
> tight bindings exist, and what breaks when you change something. Designed for
> quick interpretation by both humans and AI agents.

---

## 1. Build Orchestration Graph

```
build_and_test_all_in_docker ──────────────────────────────────────────────────
│
│  ┌──────────────────── PHASE 1: Inference Sidecars ────────────────────┐
│  │  (independent Bazel workspaces, each via bazel-debian → cbuild)     │
│  │                                                                     │
│  │  tensorflow_v2_14_0/        pytorch_v2_1_1/                         │
│  │  ├─ .bazelversion (7.4.1)   ├─ .bazelversion (7.4.1)               │
│  │  ├─ .bazelrc                ├─ .bazelrc                             │
│  │  ├─ WORKSPACE               ├─ WORKSPACE                            │
│  │  └→ artifact: binary        └→ artifact: binary                     │
│  │                                                                     │
│  └─────────────────────────────────────────────────────────────────────┘
│                          ↓ artifacts on disk
│  ┌──────────────────── PHASE 2: Main Build ────────────────────────────┐
│  │  (root Bazel workspace, via cbuild_debian)                          │
│  │  ├─ .bazelversion (7.4.1)                                           │
│  │  ├─ .bazelrc                                                        │
│  │  ├─ WORKSPACE                                                       │
│  │  ├─ third_party/container_deps.bzl  ← runtime base images          │
│  │  └→ dist/{service}_image.tar        ← final Docker images          │
│  └─────────────────────────────────────────────────────────────────────┘
│                          ↓ image tarballs
│  ┌──────────────────── PHASE 3: Platform Packaging ────────────────────┐
│  │  azure/ | aws/ | gcp/                                               │
│  └─────────────────────────────────────────────────────────────────────┘

Shared across all phases:
  Host: ~/.cache/bazel  ←──volume-mount──→  Container: /bazel_root
  Host: repo root       ←──volume-mount──→  Container: /src/workspace
```

---

## 2. Root Workspace Dependency Tree

```
ROOT WORKSPACE (.bazelversion=7.4.1)
│
├── io_bazel_rules_docker v0.26.0         ← Docker image build rules
│   └→ container_repositories(), rules_docker_deps()
│
├── google_privacysandbox_servers_common   ← DATA PLANE SHARED LIB (large dep tree)
│   ├→ cpp_dependencies()                  ← grpc, protobuf, abseil, boringssl, etc.
│   ├→ deps1() ─→ grpc_deps()             ← rules_apple, rules_go, upb
│   ├→ deps2() ─→ go toolchains, gazelle
│   ├→ deps3() ─→ rules_js, rules_ts, aspect_*
│   └→ deps4() ─→ additional transitive deps
│
├── container_deps.bzl                     ← RUNTIME IMAGES
│   ├── distroless/cc-debian12 (amd64/arm64 digests)
│   └── envoyproxy/envoy-distroless:v1.31.4 (amd64/arm64 digests)
│
├── local_repository: inference_common     ← ref to services/inference_sidecar/common
├── local_repository: pytorch_v2_1_1      ← ref to pytorch module (for main build refs)
├── local_repository: tensorflow_v2_14_0  ← ref to TF module (for main build refs)
│
├── libcbor v0.10.2, libevent v2.1.12, cddl v0.9.4
├── service_value_key_fledge_privacysandbox v1.1.0
├── com_google_cpp_proto_builder v0.1.0
├── pybind11_bazel, rules_rust/crate_universe
│
├→ sandboxed_api() → sapi_deps()
└→ init_cpp_pb_external_repositories()
```

---

## 3. TensorFlow Sidecar Dependency Tree (MOST COMPLEX)

```
TF SIDECAR WORKSPACE (.bazelversion=7.4.1)
│
│   ┌──────────────── OVERRIDE BLOCK (defined before TF macros) ──────┐
│   │  build_bazel_rules_apple  v3.1.1   ← Bazel 7 compatible        │
│   │  build_bazel_apple_support v1.11.1  ← Bazel 7 compatible       │
│   └── These override TF's pinned 2.3.0/1.6.0 via maybe semantics ──┘
│
├── org_tensorflow v2.14.0  ═══════════════╗
│   ║  (TIGHTLY COUPLED DEPENDENCY TREE)   ║
│   ║                                       ║
│   ║  tf_workspace3()                      ║
│   ║  ├─ rules_cc, rules_proto, platforms  ║
│   ║                                       ║
│   ║  tf_workspace2()                      ║  ← LARGEST DEP SURFACE
│   ║  ├─ com_github_grpc_grpc  ═══════╗   ║
│   ║  │   ║ (commit b54a5b3)          ║   ║
│   ║  │   ║ grpc_build_system.bzl     ║   ║
│   ║  │   ║ └─ UNCONDITIONAL load:   ║   ║
│   ║  │   ║    rules_apple/ios.bzl    ║   ║
│   ║  │   ╚═══════════════════════════╝   ║
│   ║  ├─ build_bazel_rules_apple  (2.3.0) ║  ← SKIPPED (overridden)
│   ║  ├─ build_bazel_apple_support (1.6.0)║  ← SKIPPED (overridden)
│   ║  ├─ com_google_protobuf              ║
│   ║  ├─ com_google_absl                  ║
│   ║  ├─ rules_go, upb                    ║
│   ║  ├─ eigen, onednn, farmhash          ║
│   ║  ├─ llvm-project, snappy, zlib       ║
│   ║  ├─ gif, png, libjpeg_turbo          ║
│   ║  └─ ~80 more deps                    ║
│   ║                                       ║
│   ║  tf_workspace1(with_rules_cc=False)   ║
│   ║  ├→ grpc_deps()                       ║
│   ║  │  └─ rules_apple 3.1.1 (SKIPPED,   ║
│   ║  │     already defined by override)   ║
│   ║  └→ benchmark_deps()                  ║
│   ║                                       ║
│   ║  tf_workspace0()                      ║
│   ║  ├→ apple_rules_dependencies()        ║
│   ║  ├→ grpc_extra_deps()                 ║
│   ║  └→ rules_foreign_cc_dependencies()   ║
│   ╚═══════════════════════════════════════╝
│
├── google_privacysandbox_servers_common   ← same data plane shared lib
│   └→ cpp_dependencies(), deps1()
│
├── inference_common (local_repository ../../common)
├── com_google_sandboxed_api → sapi_deps()
├── rapidjson v1.1.0
└── org_kernel_libcap v2.27
```

---

## 4. PyTorch & Common Sidecar Dependency Trees

```
PYTORCH SIDECAR WORKSPACE (.bazelversion=7.4.1)
│
├── pytorch_v2_1_1_deps1/deps2()          ← PyTorch-specific deps (fbgemm, etc.)
├── google_privacysandbox_servers_common
│   ├→ cpp_dependencies()                  ← grpc, protobuf, abseil
│   └→ deps1()
├→ grpc_deps()                             ← defines rules_apple 3.1.1 (no conflict)
├→ protobuf_deps()
├── inference_common (local_repository)
├── com_google_sandboxed_api → sapi_deps()
├── pybind11_bazel → python_configure
├── rapidjson v1.1.0
└── rules_fuzzing

COMMON SIDECAR WORKSPACE (.bazelversion=7.4.1)
│
├── google_privacysandbox_servers_common
│   ├→ cpp_dependencies()
│   ├→ deps1(), deps2(), deps3()
├→ grpc_deps()                             ← defines rules_apple 3.1.1 (no conflict)
├→ protobuf_deps()
└── rapidjson v1.1.0
```

---

## 5. Runtime Docker Image Layer Stack

```
FINAL SERVICE DOCKER IMAGE (e.g., buyer_frontend_service_image.tar)
┌─────────────────────────────────────────────────────────┐
│  Service binary layer                                    │  ← compiled by Bazel
│  (statically linked C++ binary + inference artifacts)    │
├─────────────────────────────────────────────────────────┤
│  envoy-distroless:v1.31.4 (FLATTENED)                   │  ← container_deps.bzl
│  ├─ Envoy proxy binary                                   │
│  ├─ /etc/os-release (from envoy's base)                  │
│  ├─ glibc, libssl3, libcrypto3 (from envoy's base)      │
│  └─ ⚠ THIS LAYER WINS on conflicts with base below      │
├─────────────────────────────────────────────────────────┤
│  distroless/cc-debian12 (BASE)                           │  ← container_deps.bzl
│  ├─ Minimal Debian 12 (bookworm) runtime                 │
│  ├─ glibc 2.36, libssl3/libcrypto3 (OpenSSL 3.0.x)      │
│  ├─ libstdc++, libgcc_s                                  │
│  └─ No shell, no package manager                         │
└─────────────────────────────────────────────────────────┘

⚠  CRITICAL: Both image layers must use the same Debian generation.
   If envoy is debian11 but base is debian12, you get debian11 libs in the
   final image because the flattened envoy layer overwrites the base files.
```

---

## 6. Shared Bazel Cache Structure

```
~/.cache/bazel/  (HOST)  ←──volume-mount──→  /bazel_root/  (CONTAINER)
│
├── build_ubuntu_{HASH_A}/                 ← inference sidecar output user root
│   ├── install/
│   │   └── {bazel_version_hash}/          ← Bazel install base
│   │       ├── A-server.jar               ← Bazel server
│   │       └── embedded_tools/jdk/        ← ⚠ EMBEDDED JDK (version = Bazel version)
│   ├── cache/                             ← repo cache (shared downloads)
│   └── {output_base_hash}/
│       └── external/                      ← fetched WORKSPACE deps for sidecars
│
├── build_ubuntu_{HASH_B}/                 ← main build output user root
│   ├── install/
│   │   └── {bazel_version_hash}/          ← (same structure as above)
│   ├── cache/
│   └── {output_base_hash}/
│       └── external/                      ← fetched WORKSPACE deps for main build
│
⚠  Different Bazel versions create different install bases under install/.
   Each install base embeds its own JDK. Bazel 6.x → JDK 11, Bazel 7.x → JDK 21.
   ALL install bases persist on disk and are visible to VAPT scans.
```

---

## 7. Tight Coupling & Rigidity Map

```
LEGEND:  ═══ Tight coupling (version-locked, breaks if mismatched)
         ─── Loose coupling (independently upgradable)
         ⚠   Known rigidity / upgrade hazard

═══════════════════════════════════════════════════════════════
COUPLING ZONE 1: Bazel ↔ Starlark API surface
═══════════════════════════════════════════════════════════════

  .bazelversion ═══ Bazel binary ═══ Embedded JDK version
       │                  │
       │            ═══ Starlark API (apple_common, cfg=host, etc.)
       │                  │
       ╠═══ rules_docker (must support Bazel version)
       ╠═══ rules_apple (multi_arch_split removed in Bazel 7) ⚠
       ╠═══ rules_cc, rules_proto, rules_java (API changes per major)
       └═══ .bazelrc flags (bzlmod, parse_headers, etc.) ⚠

  RIGIDITY: Changing .bazelversion can break ANY external dep that uses
  deprecated Starlark APIs. Test by building, read error, trace dep, override.

═══════════════════════════════════════════════════════════════
COUPLING ZONE 2: TensorFlow ↔ Transitive Deps (HIGHEST RIGIDITY)
═══════════════════════════════════════════════════════════════

  org_tensorflow v2.14.0
       │
       ╠═══ com_github_grpc_grpc (pinned commit) ⚠
       │         ╠═══ build_bazel_rules_apple (loaded unconditionally)
       │         ╠═══ com_google_protobuf
       │         └═══ upb (pinned commit, patched)
       │
       ╠═══ com_google_absl (pinned commit)
       ╠═══ eigen_archive, onednn, llvm-project
       ╠═══ ~80 more pinned deps via tf_workspace{0,1,2,3}()
       │
       └═══ bazel_toolchains, rules_cuda, rules_foreign_cc

  RIGIDITY: TF 2.14.0 pins exact commits/versions for ~100 deps.
  tf_http_archive uses maybe semantics (first-define-wins).
  Upgrade path: define newer version BEFORE tf_workspace{N}() call.
  Nuclear option: upgrade TF version (major effort, new dep tree).

═══════════════════════════════════════════════════════════════
COUPLING ZONE 3: Runtime Images ↔ Debian Generation
═══════════════════════════════════════════════════════════════

  container_deps.bzl
       │
       ╠═══ distroless/cc-debian{N}  ═══ glibc version ═══ ABI compat
       │                                       │
       │                              ═══ service binary (compiled against
       │                                   build container's glibc headers)
       │
       ╠═══ envoy-distroless:vX.Y.Z ═══ envoy's own base Debian version ⚠
       │         │
       │         └── MUST match distroless base Debian generation
       │
       └── rules_docker (container_pull, container_flatten)

  RIGIDITY: distroless base and envoy image MUST use same Debian generation.
  Service binary compiled in build-debian container MUST be ABI-compatible
  with distroless runtime glibc (forward-compatible, not backward).

═══════════════════════════════════════════════════════════════
COUPLING ZONE 4: Data Plane Shared Library ↔ All Workspaces
═══════════════════════════════════════════════════════════════

  google_privacysandbox_servers_common (pinned git commit)
       │
       ╠═══ Root workspace ─── cpp_deps, deps1-4
       ╠═══ TF sidecar    ─── cpp_deps, deps1
       ╠═══ PyTorch sidecar── cpp_deps, deps1
       ╠═══ Common sidecar ─── cpp_deps, deps1-3
       │
       └→ Transitively defines: grpc, protobuf, abseil, boringssl,
          rules_cc, rules_proto, sandboxed_api, and more

  RIGIDITY: Single version shared across all 4 workspaces.
  Upgrading it changes transitive deps for everything.
  Each workspace may pin overlapping deps at different commits → conflicts.

═══════════════════════════════════════════════════════════════
COUPLING ZONE 5: 4 Independent Workspaces ↔ Shared Cache
═══════════════════════════════════════════════════════════════

  Workspace 1 (root)           ─┐
  Workspace 2 (TF sidecar)     ├── All share ~/.cache/bazel
  Workspace 3 (PyTorch sidecar)├── via Docker volume mount
  Workspace 4 (Common sidecar) ─┘
       │
       └── Different .bazelversion → different install bases → different JDKs
           All coexist on disk. VAPT scans see everything.

  RIGIDITY: Must keep all 4 on same Bazel major version, or accept
  that old JDK persists in cache. No built-in cache isolation.
```

---

## 8. Upgrade Risk Matrix

| What You Change | Direct Impact | Transitive Risk | Rollback Ease |
|---|---|---|---|
| `container_deps.bzl` digests | Runtime image libs | Low (self-contained) | Easy (revert digests) |
| `.bazelversion` (root only) | Main build JDK, Starlark API | Medium (rules_docker, rules_cc) | Easy (revert version) |
| `.bazelversion` (all 4) | All JDKs, all Starlark APIs | **High** (TF dep tree) | Medium (must revert 4+files) |
| `rules_apple` override | TF sidecar loading | Low (iOS rules, unused on Linux) | Easy |
| `org_tensorflow` version | **Entire TF dep tree** | **Very High** (~100 pinned deps) | Hard (cascading changes) |
| `google_privacysandbox_servers_common` | All 4 workspaces transitively | **High** (grpc, protobuf, abseil) | Medium |
| `com_github_grpc_grpc` version | protobuf, upb, rules_apple | **High** (ABI + Starlark) | Hard |
| `envoy-distroless` version | Runtime image only | Medium (Debian gen must match) | Easy |
| `builders/images/build-debian` | Build container, not runtime | Low (compilation env) | Easy |

---

## 9. Dependency Version Quick Reference

| Dependency | Version/Ref | Defined In | Used By |
|---|---|---|---|
| Bazel | 7.4.1 | `.bazelversion` (×4) | All workspaces |
| JDK (embedded) | 21.0.4 | Bazel 7.4.1 binary | Build process |
| JDK (remote toolchain) | 21.0.3 | `.bazelrc` remotejdk_21 | Build process |
| rules_docker | v0.26.0 | Root `WORKSPACE` | Main build only |
| distroless base | cc-debian12 | `container_deps.bzl` | Runtime images |
| Envoy sidecar | v1.31.4 | `container_deps.bzl` | Runtime images |
| glibc (runtime) | 2.36 | distroless/cc-debian12 | Shipped images |
| OpenSSL (runtime) | 3.0.x (libssl3) | distroless + envoy | Shipped images |
| TensorFlow | 2.14.0 | TF sidecar `WORKSPACE` | TF sidecar only |
| PyTorch | 2.1.1 | PyTorch sidecar deps | PyTorch sidecar only |
| gRPC (TF) | commit b54a5b3 | TF's `workspace2.bzl` | TF sidecar |
| gRPC (others) | via data_plane_shared | Root, PyTorch, Common | All except TF |
| rules_apple | 3.1.1 | TF `WORKSPACE` override | TF sidecar (loaded by gRPC) |
| apple_support | 1.11.1 | TF `WORKSPACE` override | TF sidecar |
| protobuf | via TF / data_plane | All workspaces | All workspaces |
| abseil-cpp | commit f845e60 (TF) | TF sidecar `WORKSPACE` | TF sidecar |
| data-plane-shared-libs | commit a7515f8 | All 4 `WORKSPACE` files | All workspaces |
| BoringSSL | via data_plane/grpc | Build-time only | All workspaces |
| sandboxed_api | via data_plane | All workspaces | All workspaces |
