
#pragma once
#include <boost/algorithm/string/replace.hpp>
#include <ff/functionflow.h>
#include <libubre/common/common.h>
#include <libubre/common/configuration.h>
#include <libubre/common/version.h>
#include <libubre/core/command.h>
#include <libubre/core/net_ipc/ipc_interface.h>
#include <libubre/core/net_ipc/nipc_pkg.h>
#include <libubre/fs/bc_storage_session.h>
#include <libubre/fs/proto/block.pb.h>
#include <libubre/fs/proto/ir.pb.h>
#include <libubre/fs/proto/trie.pb.h>
#include <libubre/util/bc_generator.h>
#include <libubre/util/quitable_thread.h>
#include <libubre/util/singleton.h>

using bc_storage_session = neb::fs::bc_storage_session;
using generate_block = neb::util::generate_block;
using all_accounts = neb::util::all_accounts;
using nas = neb::nas;
using address_t = neb::address_t;
using block_height_t = neb::block_height_t;

