// Copyright (c) 2023 OwnCrew Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "nontrivial-threadlocal.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class OwnCrewModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<owncrew::NonTrivialThreadLocal>("owncrew-nontrivial-threadlocal");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<OwnCrewModule>
    X("owncrew-module", "Adds owncrew checks.");

volatile int OwnCrewModuleAnchorSource = 0;
