# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libowncrew_cli*         | RPC client functionality used by *owncrew-cli* executable |
| *libowncrew_common*      | Home for common functionality shared by different executables and libraries. Similar to *libowncrew_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libowncrew_consensus*   | Consensus functionality used by *libowncrew_node* and *libowncrew_wallet*. |
| *libowncrew_crypto*      | Hardware-optimized functions for data encryption, hashing, message authentication, and key derivation. |
| *libowncrew_kernel*      | Consensus engine and support library used for validation by *libowncrew_node*. |
| *libowncrewqt*           | GUI functionality used by *owncrew-qt* and *owncrew-gui* executables. |
| *libowncrew_ipc*         | IPC functionality used by *owncrew-node*, *owncrew-wallet*, *owncrew-gui* executables to communicate when [`-DWITH_MULTIPROCESS=ON`](multiprocess.md) is used. |
| *libowncrew_node*        | P2P and RPC server functionality used by *owncrewd* and *owncrew-qt* executables. |
| *libowncrew_util*        | Home for common functionality shared by different executables and libraries. Similar to *libowncrew_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libowncrew_wallet*      | Wallet functionality used by *owncrewd* and *owncrew-wallet* executables. |
| *libowncrew_wallet_tool* | Lower-level wallet functionality used by *owncrew-wallet* executable. |
| *libowncrew_zmq*         | [ZeroMQ](../zmq.md) functionality used by *owncrewd* and *owncrew-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. An exception is *libowncrew_kernel*, which, at some future point, will have a documented external interface.

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`add_library(owncrew_* ...)`](../../src/CMakeLists.txt) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libowncrew_node* code lives in `src/node/` in the `node::` namespace
  - *libowncrew_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libowncrew_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libowncrew_util* code lives in `src/util/` in the `util::` namespace
  - *libowncrew_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

owncrew-cli[owncrew-cli]-->libowncrew_cli;

owncrewd[owncrewd]-->libowncrew_node;
owncrewd[owncrewd]-->libowncrew_wallet;

owncrew-qt[owncrew-qt]-->libowncrew_node;
owncrew-qt[owncrew-qt]-->libowncrewqt;
owncrew-qt[owncrew-qt]-->libowncrew_wallet;

owncrew-wallet[owncrew-wallet]-->libowncrew_wallet;
owncrew-wallet[owncrew-wallet]-->libowncrew_wallet_tool;

libowncrew_cli-->libowncrew_util;
libowncrew_cli-->libowncrew_common;

libowncrew_consensus-->libowncrew_crypto;

libowncrew_common-->libowncrew_consensus;
libowncrew_common-->libowncrew_crypto;
libowncrew_common-->libowncrew_util;

libowncrew_kernel-->libowncrew_consensus;
libowncrew_kernel-->libowncrew_crypto;
libowncrew_kernel-->libowncrew_util;

libowncrew_node-->libowncrew_consensus;
libowncrew_node-->libowncrew_crypto;
libowncrew_node-->libowncrew_kernel;
libowncrew_node-->libowncrew_common;
libowncrew_node-->libowncrew_util;

libowncrewqt-->libowncrew_common;
libowncrewqt-->libowncrew_util;

libowncrew_util-->libowncrew_crypto;

libowncrew_wallet-->libowncrew_common;
libowncrew_wallet-->libowncrew_crypto;
libowncrew_wallet-->libowncrew_util;

libowncrew_wallet_tool-->libowncrew_wallet;
libowncrew_wallet_tool-->libowncrew_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class owncrew-qt,owncrewd,owncrew-cli,owncrew-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Crypto* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus, crypto, and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libowncrew_wallet* and *libowncrew_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libowncrew_crypto* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libowncrew_consensus* should only depend on *libowncrew_crypto*, and all other libraries besides *libowncrew_crypto* should be allowed to depend on it.

- *libowncrew_util* should be a standalone dependency that any library can depend on, and it should not depend on other libraries except *libowncrew_crypto*. It provides basic utilities that fill in gaps in the C++ standard library and provide lightweight abstractions over platform-specific features. Since the util library is distributed with the kernel and is usable by kernel applications, it shouldn't contain functions that external code shouldn't call, like higher level code targeted at the node or wallet. (*libowncrew_common* is a better place for higher level code, or code that is meant to be used by internal applications only.)

- *libowncrew_common* is a home for miscellaneous shared code used by different OwnCrew Core applications. It should not depend on anything other than *libowncrew_util*, *libowncrew_consensus*, and *libowncrew_crypto*.

- *libowncrew_kernel* should only depend on *libowncrew_util*, *libowncrew_consensus*, and *libowncrew_crypto*.

- The only thing that should depend on *libowncrew_kernel* internally should be *libowncrew_node*. GUI and wallet libraries *libowncrewqt* and *libowncrew_wallet* in particular should not depend on *libowncrew_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libowncrew_consensus*, *libowncrew_common*, *libowncrew_crypto*, and *libowncrew_util*, instead of *libowncrew_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libowncrewqt*, *libowncrew_node*, *libowncrew_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](../../src/interfaces/) abstract interfaces.

## Work in progress

- Validation code is moving from *libowncrew_node* to *libowncrew_kernel* as part of [The libowncrewkernel Project #27587](https://github.com/owncrew/owncrew/issues/27587)
