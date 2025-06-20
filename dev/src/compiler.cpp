// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// lobster.cpp : Defines the entry point for the console application.
//
#include "lobster/stdafx.h"

#include "lobster/il.h"
#include "lobster/natreg.h"

#include "lobster/lex.h"
#include "lobster/idents.h"
#include "lobster/node.h"

#include "lobster/compiler.h"

#include "lobster/parser.h"
#include "lobster/typecheck.h"
#include "lobster/constval.h"
#include "lobster/optimizer.h"
#include "lobster/tonative.h"
#include "lobster/codegen.h"

SlabAlloc *g_current_slaballoc = nullptr;

namespace lobster {

const Type g_type_int(V_INT);
const Type g_type_float(V_FLOAT);
const Type g_type_string(V_STRING);
const Type g_type_any(V_ANY);
const Type g_type_vector_any(V_VECTOR, &g_type_any);
const Type g_type_vector_int(V_VECTOR, &g_type_int);
const Type g_type_vector_float(V_VECTOR, &g_type_float);
const Type g_type_function_null_void(V_FUNCTION);  // no args, void return.
const Type g_type_resource(V_RESOURCE);
const Type g_type_vector_resource(V_VECTOR, &g_type_resource);
const Type g_type_typeid(V_TYPEID, &g_type_any);
const Type g_type_void(V_VOID);
const Type g_type_undefined(V_UNDEFINED);

TypeRef type_int = &g_type_int;
TypeRef type_float = &g_type_float;
TypeRef type_string = &g_type_string;
TypeRef type_any = &g_type_any;
TypeRef type_vector_int = &g_type_vector_int;
TypeRef type_vector_float = &g_type_vector_float;
TypeRef type_function_null_void = &g_type_function_null_void;
TypeRef type_resource = &g_type_resource;
TypeRef type_vector_resource = &g_type_vector_resource;
TypeRef type_typeid = &g_type_typeid;
TypeRef type_void = &g_type_void;
TypeRef type_undefined = &g_type_undefined;

const Type g_type_vector_string(V_VECTOR, &g_type_string);
const Type g_type_vector_vector_int(V_VECTOR, &g_type_vector_int);
const Type g_type_vector_vector_float(V_VECTOR, &g_type_vector_float);
const Type g_type_vector_vector_vector_float(V_VECTOR, &g_type_vector_vector_float);

NumStruct g_ns_int_unknown{ V_INT, -1 };
NumStruct g_ns_float_unknown{ V_FLOAT, -1 };

NumStruct g_ns_int[] = { { V_INT, 1 }, { V_INT, 2 }, { V_INT, 3 }, { V_INT, 4 } };
NumStruct g_ns_float[] = { { V_FLOAT, 1 }, { V_FLOAT, 2 }, { V_FLOAT, 3 }, { V_FLOAT, 4 } };

const Type g_type_ns_int_unknown(&g_ns_int_unknown);
const Type g_type_ns_float_unknown(&g_ns_float_unknown);

const Type g_type_ns_int[] = { &g_ns_int[0], &g_ns_int[1], &g_ns_int[2], &g_ns_int[3] };
const Type g_type_ns_float[] = { &g_ns_float[0], &g_ns_float[1], &g_ns_float[2], &g_ns_float[3] };

const Type g_type_vector_ns_int[] = {
    { V_VECTOR, &g_type_ns_int_unknown },
    { V_VECTOR, &g_type_ns_int_unknown },
    { V_VECTOR, &g_type_ns_int[0] },
    { V_VECTOR, &g_type_ns_int[1] },
    { V_VECTOR, &g_type_ns_int[2] },
    { V_VECTOR, &g_type_ns_int[3] }
};
const Type g_type_vector_ns_float[] = {
    { V_VECTOR, &g_type_ns_float_unknown },
    { V_VECTOR, &g_type_ns_float_unknown },
    { V_VECTOR, &g_type_ns_float[0] },
    { V_VECTOR, &g_type_ns_float[1] },
    { V_VECTOR, &g_type_ns_float[2] },
    { V_VECTOR, &g_type_ns_float[3] }
};
const Type g_type_vector_vector_ns_int[] = {
    { V_VECTOR, &g_type_vector_ns_int[0] },
    { V_VECTOR, &g_type_vector_ns_int[1] },
    { V_VECTOR, &g_type_vector_ns_int[2] },
    { V_VECTOR, &g_type_vector_ns_int[3] },
    { V_VECTOR, &g_type_vector_ns_int[4] },
    { V_VECTOR, &g_type_vector_ns_int[5] }
};
const Type g_type_vector_vector_ns_float[] = {
    { V_VECTOR, &g_type_vector_ns_float[0] },
    { V_VECTOR, &g_type_vector_ns_float[1] },
    { V_VECTOR, &g_type_vector_ns_float[2] },
    { V_VECTOR, &g_type_vector_ns_float[3] },
    { V_VECTOR, &g_type_vector_ns_float[4] },
    { V_VECTOR, &g_type_vector_ns_float[5] }
};

ResourceType *g_resource_type_list = nullptr;

TypeRef WrapKnown(UnTypeRef elem, ValueType with) {
    if (with == V_VECTOR) {
        switch (elem->t) {
            case V_ANY:      return &g_type_vector_any;
            case V_INT:      return elem->e ? nullptr : type_vector_int;
            case V_FLOAT:    return type_vector_float;
            case V_STRING:   return &g_type_vector_string;
            case V_RESOURCE: return &elem->rt->thistypevec;
            case V_VECTOR:   switch (elem->sub->t) {
                case V_INT:    return elem->sub->e ? nullptr : &g_type_vector_vector_int;
                case V_FLOAT:  return &g_type_vector_vector_float;
                case V_VECTOR: switch (elem->sub->sub->t) {
                    case V_FLOAT: return &g_type_vector_vector_vector_float;
                    default:      return nullptr;
                }
                case V_STRUCT_NUM: {
                    auto ns = elem->sub->ns;
                    assert((ns->flen >= 1 && ns->flen <= 4) || ns->flen == -1);
                    return &(ns->t == V_INT ? g_type_vector_vector_ns_int
                                            : g_type_vector_vector_ns_float)[ns->flen + 1];
                }
                default:
                    return nullptr;
            }
            case V_STRUCT_NUM: {
                auto ns = elem->ns;
                assert((ns->flen >= 1 && ns->flen <= 4) || ns->flen == -1);
                return &(ns->t == V_INT ? g_type_vector_ns_int
                                        : g_type_vector_ns_float)[ns->flen + 1];
            }
            default:
                return nullptr;
        }
    } else if (with == V_NIL) {
        switch (elem->t) {
            case V_ANY:       { static const Type t(V_NIL, &g_type_any); return &t; }
            case V_INT:       { static const Type t(V_NIL, &g_type_int); return elem->e ? nullptr : &t; }
            case V_FLOAT:     { static const Type t(V_NIL, &g_type_float); return &t; }
            case V_STRING:    { static const Type t(V_NIL, &g_type_string); return &t; }
            //case V_FUNCTION:  { static const Type t(V_NIL, &g_type_function_null); return &t; }
            case V_RESOURCE:  { return &elem->rt->thistypenil; }
            case V_VECTOR: switch (elem->sub->t) {
                case V_INT:    { static const Type t(V_NIL, &g_type_vector_int); return elem->sub->e ? nullptr : &t; }
                case V_FLOAT:  { static const Type t(V_NIL, &g_type_vector_float); return &t; }
                case V_STRING: { static const Type t(V_NIL, &g_type_vector_string); return &t; }
                default: return nullptr;
            }
            default: return nullptr;
        }
    } else if (with == V_STRUCT_NUM) {
        switch (elem->t) {
            case V_INT:      return &g_type_ns_int_unknown;
            case V_FLOAT:    return &g_type_ns_float_unknown;
            default: return nullptr;
        }
    } else {
        return nullptr;
    }
}

TypeRef FixedNumStruct(ValueType num, int flen) {
    assert(flen >= 2 && flen <= 4);
    return &(num == V_INT ? g_type_ns_int : g_type_ns_float)[flen - 1];
}

bool IsCompressed(string_view filename) {
    auto dot = filename.find_last_of('.');
    if (dot == string_view::npos) return false;
    auto ext = filename.substr(dot);
    return ext == ".c" || ext == ".lbc" || ext == ".lobster" || ext == ".materials" || ext == ".glsl";
}

static const uint8_t *magic = (uint8_t *)"LPAK";
static const size_t magic_size = 4;
static const size_t header_size = magic_size + sizeof(int64_t) * 4;
static const char *mdname = "metadata.lbc";
static const char *ccname = "c_codegen.c";
static const int64_t current_version = 2;

template <typename T> int64_t LE(T x) { return flatbuffers::EndianScalar((int64_t)x); };

string BuildPakFile(string &pakfile, string &metadata_buffer, set<string> &files, uint64_t src_hash,
                    const string &c_codegen) {
    // All offsets in 64bit, just in-case we ever want pakfiles > 4GB :)
    // Since we're building this in memory, they can only be created by a 64bit build.
    vector<int64_t> filestarts;
    vector<int64_t> namestarts;
    vector<int64_t> uncompressed;
    vector<string> filenames;
    auto add_file = [&](string_view buf, string_view filename) {
        filestarts.push_back(LE(pakfile.size()));
        filenames.push_back(string(filename));
        LOG_INFO("adding to pakfile: ", filename);
        if (IsCompressed(filename)) {
            string out;
            WEntropyCoder<true>((uint8_t *)buf.data(), buf.length(), buf.length(), out);
            pakfile += out;
            uncompressed.push_back(buf.length());
        } else {
            pakfile += buf;
            uncompressed.push_back(-1);
        }
    };
    // Start with a magic id, just for the hell of it.
    pakfile.insert(pakfile.end(), magic, magic + magic_size);
    // Metadata always first entry.
    add_file(metadata_buffer, mdname);
    if (!c_codegen.empty()) {
        add_file(c_codegen, ccname);
    }
    // Followed by all files.
    files.insert("data/shaders/default.materials");  // If it hadn't already been added.
    string buf;
    function<string(const string &)> addrec;
    addrec = [&](const string &filename) -> string {
        auto l = LoadFile(filename, &buf);
        if (l >= 0) {
            add_file(buf, filename);
        } else {
            auto base = filename;
            auto pat = string{};
            auto pos = filename.find("#");
            if (pos != filename.npos) {
                pat = filename.substr(pos + 1);
                base = filename.substr(0, pos);
            }
            vector<DirectoryInfo> dir;
            if (!ScanDir(base, dir)) return "cannot load file/dir for pakfile: " + filename;
            for (auto &entry : dir) {
                if (!pat.empty() && entry.name.find(pat) == entry.name.npos) continue;
                auto fn = base;
                if (fn.back() != '/') fn += "/";
                fn += entry.name;
                auto err = addrec(fn);
                if (!err.empty()) return err;
            }
        }
        return "";
    };
    for (auto &filename : files) {
        auto err = addrec(filename);
        if (!err.empty()) return err;
    }
    // Now we can write the directory, first the names:
    auto dirstart = LE(pakfile.size());
    for (auto &filename : filenames) {
        namestarts.push_back(LE(pakfile.size()));
        pakfile.insert(pakfile.end(), filename.c_str(), filename.c_str() + filename.length() + 1);
    }
    // Then the starting offsets and other data:
    pakfile.insert(pakfile.end(), (uint8_t *)uncompressed.data(),
        (uint8_t *)(uncompressed.data() + uncompressed.size()));
    pakfile.insert(pakfile.end(), (uint8_t *)filestarts.data(),
        (uint8_t *)(filestarts.data() + filestarts.size()));
    pakfile.insert(pakfile.end(), (uint8_t *)namestarts.data(),
        (uint8_t *)(namestarts.data() + namestarts.size()));
    auto num = LE(filestarts.size());
    // Finally the "header" (or do we call this a "tailer" ? ;)
    auto header_start = pakfile.size();
    auto version = LE(current_version);
    auto src_hash_le = LE(src_hash);
    pakfile.insert(pakfile.end(), (uint8_t *)&src_hash_le, (uint8_t *)(&src_hash_le + 1));
    pakfile.insert(pakfile.end(), (uint8_t *)&num, (uint8_t *)(&num + 1));
    pakfile.insert(pakfile.end(), (uint8_t *)&dirstart, (uint8_t *)(&dirstart + 1));
    pakfile.insert(pakfile.end(), (uint8_t *)&version, (uint8_t *)(&version + 1));
    pakfile.insert(pakfile.end(), magic, magic + magic_size);
    assert(pakfile.size() - header_start == header_size);
    (void)header_start;
    return "";
}

// This just loads the directory part of a pakfile such that subsequent LoadFile calls know how
// to load from it.
bool LoadPakDir(const char *lpak, uint64_t &src_hash_dest) {
    // This supports reading from a pakfile > 4GB even on a 32bit system! (as long as individual
    // files in it are <= 4GB).
    auto plen = LoadFile(lpak, nullptr, 0, 0);
    if (plen < 0) return false;
    string header;
    if (LoadFile(lpak, &header, plen - (int64_t)header_size, header_size) < 0 ||
        memcmp(header.c_str() + header_size - magic_size, magic, magic_size)) return false;
    auto read_unaligned64 = [](const void *p) {
        int64_t r;
        memcpy(&r, p, sizeof(int64_t));
        return LE(r);
    };
    auto src_hash = (uint64_t)read_unaligned64((int64_t *)header.c_str());
    auto num = read_unaligned64((int64_t *)header.c_str() + 1);
    auto dirstart = read_unaligned64((int64_t *)header.c_str() + 2);
    auto version = read_unaligned64((int64_t *)header.c_str() + 3);
    if (version != current_version) return false;
    if (dirstart > plen) return false;
    string dir;
    if (LoadFile(lpak, &dir, dirstart, plen - dirstart - (int64_t)header_size) < 0)
        return false;
    auto namestarts = (int64_t *)(dir.c_str() + dir.length()) - num;
    auto filestarts = namestarts - num;
    auto uncompressed = filestarts - num;
    for (int64_t i = 0; i < num; i++) {
        auto name = string_view(dir.c_str() + (read_unaligned64(namestarts + i) - dirstart));
        auto off = read_unaligned64(filestarts + i);
        auto end = i < num + 1 ? read_unaligned64(filestarts + i + 1) : dirstart;
        auto len = end - off;
        LOG_INFO("pakfile dir: ", name, " : ", len);
        AddPakFileEntry(lpak, name, off, len, read_unaligned64(uncompressed + i));
    }
    src_hash_dest = src_hash;
    return true;
}

bool LoadMetaDataAndCode(string &metadata, string &c_codegen) {
    if (LoadFile(mdname, &metadata) < 0) return false;
    LoadFile(ccname, &c_codegen);
    flatbuffers::Verifier verifier((const uint8_t *)metadata.c_str(), metadata.length());
    auto ok = metadata::VerifyMetadataFileBuffer(verifier);
    assert(ok);
    return ok;
}

void RegisterBuiltin(NativeRegistry &nfr, const char *ns, const char *name,
                     void (* regfun)(NativeRegistry &)) {
    LOG_DEBUG("subsystem: ", name);
    nfr.NativeSubSystemStart(ns, name);
    regfun(nfr);
}

void DumpBuiltinNames(NativeRegistry &nfr) {
    string s;
    for (auto nf : nfr.nfuns) {
        if (nfr.subsystems[nf->subsystemid] == "plugin") continue;
        s += nf->name;
        s += "|";
    }
    WriteFile("builtin_functions_names.txt", false, s, false);
}

string JSONEscape(string_view in) {
    string s;
    flatbuffers::EscapeString(in.data(), in.size(), &s, false, false);
    return s.substr(1, s.size()-2);
}
string HTMLEscape(string_view in) {
    string s;
    for (auto c : in) {
        switch (c) {
            case '&': s += "&amp;"; break;
            case '<': s += "&lt;"; break;
            case '>': s += "&gt;"; break;
            default: s += c; break;
        }
    }
    return s;
}

enum Tags {
    Doc = 0,
    Table = 1,
    Row = 2,
    FirstRow = 3,
    Td = 4,
    Subsystem = 5,
    Name = 6,
    Params = 7,
    Help = 8,
    Font = 9,
    Returns = 10,
    ParamType = 11,
    ParamName = 12,
    ParamDefault = 13,
    Param = 14,
    RetTypeWrap = 15,
};

string GetBuiltinDoc(NativeRegistry &nfr, bool group_subsystem, string (&doc_tags)[16][2], string (*escape)(string_view)) {
    string s = doc_tags[Tags::Doc][0];
    int cursubsystem = -1;
    bool is_first_row = true;
    bool tablestarted = !group_subsystem;
    if(tablestarted) s += doc_tags[Tags::Table][0];
    for (auto nf : nfr.nfuns) {
        if (nfr.subsystems[nf->subsystemid] == "plugin") continue;
        if (group_subsystem) {
            if (nf->subsystemid != cursubsystem) {
                if (tablestarted) s += doc_tags[Tags::Table][1];
                tablestarted = false;
                if (group_subsystem) s += is_first_row ? doc_tags[Tags::FirstRow][0] : doc_tags[Tags::Row][0];
                s += cat(doc_tags[Tags::Subsystem][0], nfr.subsystems[nf->subsystemid], doc_tags[Tags::Subsystem][1]);
                if (group_subsystem) s += doc_tags[Tags::Row][1];
                cursubsystem = nf->subsystemid;
            }
            if (!tablestarted) {
                s += doc_tags[Tags::Table][0];
                tablestarted = true;
            }
        }
        s += is_first_row ? doc_tags[Tags::FirstRow][0] : doc_tags[Tags::Row][0];
        if (!group_subsystem) {
            s += cat(doc_tags[Tags::Subsystem][0],
                    nfr.subsystems[nf->subsystemid],
                    doc_tags[Tags::Subsystem][1]);
        }
        s += cat(doc_tags[Tags::Name][0], nf->name, doc_tags[Tags::Name][1]);
        s += doc_tags[Tags::Params][0];
        int last_not_optional = -1;
        for (auto [i, a] : enumerate(nf->args)) {
            if (!a.optional) last_not_optional = (int)i;
        }
        for (auto [i, a] : enumerate(nf->args)) {
            auto argname = nf->args[i].name;
            if (i) s +=  ", ";
            s += doc_tags[Tags::Param][0];
            s += cat(doc_tags[Tags::ParamName][0], argname, doc_tags[Tags::ParamName][1]);
            s += doc_tags[Tags::Font][0];
            s += doc_tags[Tags::ParamType][0];
            if (a.type->t != V_ANY) {
                s += a.flags & NF_BOOL
                    ? "bool"
                    : escape(TypeName(a.type->ElementIfNil()));
            } else {
                s += "any";
            }
            s += doc_tags[Tags::ParamType][1];
            s += doc_tags[Tags::Font][1];
            if (a.optional && (int)i > last_not_optional) {
                s += doc_tags[Tags::ParamDefault][0];
                switch (a.type->t) {
                    case V_INT:
                        if (a.flags & NF_BOOL)
                            append(s, a.default_val ? "true" : "false");
                        else
                            append(s, a.default_val);
                        break;
                    case V_FLOAT:
                        append(s, (float)a.default_val);
                        break;
                    default:
                        s += "nil";
                }
                s += doc_tags[Tags::ParamDefault][1];
            }
            s += doc_tags[Tags::Param][1];
        }
        s += doc_tags[Tags::Params][1];
        if (nf->retvals.size()) {
            s += doc_tags[Tags::Returns][0];
            for (auto [i, a] : enumerate(nf->retvals)) {
                s += doc_tags[Tags::RetTypeWrap][0];
                s += doc_tags[Tags::Font][0];
                s += escape(TypeName(a.type));
                s += doc_tags[Tags::Font][1];
                s += doc_tags[Tags::RetTypeWrap][1];
                if (i < nf->retvals.size() - 1) s += ", ";
            }
            s += doc_tags[Tags::Returns][1];
        }
        s += cat(doc_tags[Tags::Help][0], escape(nf->help), doc_tags[Tags::Help][1], "\n");
        s += doc_tags[Tags::Row][1];
        is_first_row = false;
    }
    s += doc_tags[Tags::Table][1];
    s += doc_tags[Tags::Doc][1];
    return s;
}

void DumpBuiltinDoc(NativeRegistry &nfr, bool group_subsystem) {
    string html_tags[16][2] = {
    /* Doc          */  {"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
    /*              */   "<html>\n<head>\n<title>lobster builtin function reference</title>\n"
    /*              */   "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
    /*              */   "<style type=\"text/css\">"
    /*              */   "table.a, tr.a, td.a {font-size: 10pt;border: 1pt solid #DDDDDD;"
    /*              */   " border-Collapse: collapse; max-width: 88em}</style>\n"
    /*              */   "</head>\n<body><center><table border=0><tr><td>\n<p>"
    /*              */   "lobster builtin functions:"
    /*              */   "(file auto generated by compiler, do not modify)</p></td></tr>", "\n</table></center></body>\n</html>\n"},
    /* Table        */  {"<tr><td><table class=\"a\" border=1 cellspacing=0 cellpadding=4>", "</table></td></tr>\n"},
    /* Row          */  {"<tr class=\"a\" valign=top>", "</tr>\n"},
    /* FirstRow     */  {"<tr class=\"a\" valign=top>", "</tr>\n"},
    /* Td           */  {"<td>", "</td>"},
    /* Subsystem    */  {"<td><h3>", "</h3></td>"},
    /* Name         */  {"<td class=\"a\"><tt><b>", "</b>"},
    /* Params       */  {"(", ")"},
    /* Help         */  {"</tt></td><td class=\"a\">", "</td>\n"},
    /* Font         */  {"<font color=\"#666666\">", "</font>"},
    /* Returns      */  {" -> ", ""},
    /* ParamType    */  {": ", ""},
    /* ParamName    */  {"", ""},
    /* ParamDefault */  {" = ", ""},
    /* Param        */  {"", ""},
    /* RetTypeWrap  */  {"", ""}};
    string s = GetBuiltinDoc(nfr, group_subsystem, html_tags, HTMLEscape);
    WriteFile("builtin_functions_reference.html", false, s, false);
}

void DumpBuiltinDocJson(NativeRegistry &nfr) {
    string json_tags[16][2] = {
    /* Doc          */ {"", ""},
    /* Table        */ {"[", "]"},
    /* Row          */ {",\n{", "}"},
    /* FirstRow     */ {"{", "}"},
    /* Td           */ {"{", "}"},
    /* Subsystem    */ {"\"subsystem\": \"", "\", "},
    /* Name         */ {"\"funcname\": \"", "\", "},
    /* Params       */ {"\"args\":[", "]"},
    /* Help         */ {", \"doc\": \"", "\""},
    /* Font         */ {"",""},
    /* Returns      */ {", \"returns\": [", "]"},
    /* ParamType    */ {", \"type\": \"", "\""},
    /* ParamName    */ {"\"name\": \"", "\""},
    /* ParamDefault */ {", \"default\": \"", "\""},
    /* Param        */ {"{", "}"},
    /* RetTypeWrap  */ {"\"", "\""}};
    cout << GetBuiltinDoc(nfr, false, json_tags, JSONEscape);
}

void PrepQuery(Query &query, vector<pair<string, string>> &filenames) {
    for (auto [i, fn] : enumerate(filenames)) {
        if (fn.first == query.file) {
            query.qloc.fileidx = (int)i;
            break;
        }
    }
    if (query.qloc.fileidx < 0) {
        THROW_OR_ABORT("query file not part of compilation: " + query.file);
    }
    query.qloc.line = parse_int<int>(query.line);
    query.filenames = &filenames;
}

void Compile(NativeRegistry &nfr, string_view fn, string_view stringsource, string &metadata_buffer,
             string *parsedump, string *pakfile, bool return_value, int runtime_checks,
             Query *query, int max_errors, bool full_error, bool jit_mode, string &c_codegen,
             bool code_pak, string_view custom_pre_init_name) {
    #ifdef NDEBUG
        SlabAlloc slaballoc;
        if (g_current_slaballoc) THROW_OR_ABORT("nested slab allocator use");
        g_current_slaballoc = &slaballoc;
        struct SlabReset {
            ~SlabReset() {
                g_current_slaballoc = nullptr;
            }
        } slabreset;
    #endif
    vector<pair<string, string>> filenames;
    Lex lex(fn, filenames, nfr.namespaces, stringsource, max_errors);
    SymbolTable st(lex);
    Parser parser(nfr, lex, st);
    parser.Parse();
    if (query) PrepQuery(*query, filenames);
    TypeChecker tc(parser, st, return_value, query, full_error);
    if (query) { // Failed to find location during type checking.
        if(!tc.ProcessQuery()) THROW_OR_ABORT("query_unknown_ident: " + query->iden);
    }
    if (lex.num_errors) THROW_OR_ABORT("errors encountered, aborting");
    tc.Stats(filenames);
    // Optimizer is not optional, must always run, since TypeChecker and CodeGen
    // rely on it culling const if-thens and other things.
    Optimizer opt(parser, st, tc, runtime_checks);
    if (parsedump) *parsedump = parser.DumpAll(true);
    auto src_hash = lex.HashAll();
    CodeGen cg(parser, st, return_value, runtime_checks, !jit_mode, src_hash,
               c_codegen, custom_pre_init_name);
    st.Serialize(cg.type_table, cg.sids, cg.stringtable, metadata_buffer, filenames, cg.ser_ids, src_hash);
    if (pakfile) {
        auto err = BuildPakFile(*pakfile, metadata_buffer, parser.pakfiles, src_hash,
                                code_pak ? c_codegen : string());
        if (!err.empty()) THROW_OR_ABORT(err);
    }
}

pair<string, iint> RunTCC(NativeRegistry &nfr, string_view metadata_buffer, string_view fn,
                          const char *object_name, vector<string> &&program_args,
                          bool compile_only, string &error, int runtime_checks, bool dump_leaks,
                          bool stack_trace_python_ordering, const string &c_codegen) {
    #if VM_JIT_MODE
        const char *export_names[] = { "compiled_entry_point", "vtables", nullptr };
        auto start_time = SecondsSinceStart();
        pair<string, iint> ret;
        auto ok = RunC(
            c_codegen.c_str(), object_name, error, vm_ops_jit_table, export_names,
            [&](void **exports) -> bool {
                LOG_INFO("time to tcc (seconds): ", SecondsSinceStart() - start_time);
                if (compile_only) return true;
                // Verify the bytecode.
                flatbuffers::Verifier verifier((uint8_t *)metadata_buffer.data(), metadata_buffer.size());
                auto ok = metadata::VerifyMetadataFileBuffer(verifier);
                if (!ok) THROW_OR_ABORT("metadata file failed to verify");
                auto bcf = metadata::GetMetadataFile(metadata_buffer.data());
                if (bcf->metadata_version() != LOBSTER_METADATA_FORMAT_VERSION)
                    THROW_OR_ABORT("metadata is from a different version of Lobster");
                vector<type_elem_t> type_table;
                for (flatbuffers::uoffset_t i = 0; i < bcf->typetable()->size(); i++) {
                    type_table.push_back((type_elem_t)bcf->typetable()->Get(i));
                }
                vector<string_view> stringtable;
                for (flatbuffers::uoffset_t i = 0; i < bcf->stringtable()->size(); i++) {
                    stringtable.push_back(bcf->stringtable()->Get(i)->string_view());
                }
                vector<string_view> file_names;
                for (flatbuffers::uoffset_t i = 0; i < bcf->filenames()->size(); i++) {
                    file_names.push_back(bcf->filenames()->Get(i)->string_view());
                }
                vector<string_view> function_names;
                for (flatbuffers::uoffset_t i = 0; i < bcf->functions()->size(); i++) {
                    function_names.push_back(bcf->functions()->Get(i)->name()->string_view());
                }
                vector<VMUDT> udts;
                vector<VMField> fields;
                for (flatbuffers::uoffset_t i = 0; i < bcf->udts()->size(); i++) {
                    auto udt = bcf->udts()->Get(i);
                    for (flatbuffers::uoffset_t j = 0; j < udt->fields()->size(); j++) {
                        auto field = udt->fields()->Get(j);
                        fields.push_back(VMField{ field->name()->string_view(), field->offset() });
                    }
                }
                size_t off = 0;
                for (flatbuffers::uoffset_t i = 0; i < bcf->udts()->size(); i++) {
                    auto udt = bcf->udts()->Get(i);
                    auto fspan = span(fields.data() + off, fields.data() + off + udt->fields()->size());
                    udts.push_back(VMUDT{ udt->name()->string_view(), udt->idx(), udt->size(),
                                          udt->super_idx(), udt->typeidx(), fspan });
                    off += udt->fields()->size();
                }
                vector<VMSpecIdent> specidents;
                for (flatbuffers::uoffset_t i = 0; i < bcf->specidents()->size(); i++) {
                    auto sid = bcf->specidents()->Get(i);
                    auto id = bcf->idents()->Get(sid->ididx());
                    specidents.push_back(VMSpecIdent {
                        id->name()->string_view(), sid->idx(), sid->typeidx(),
                        sid->used_as_freevar(), id->readonly(), id->global() });
                }
                vector<VMEnum> enums;
                vector<VMEnumVal> enumvals;
                for (flatbuffers::uoffset_t i = 0; i < bcf->enums()->size(); i++) {
                    auto e = bcf->enums()->Get(i);
                    for (flatbuffers::uoffset_t j = 0; j < e->vals()->size(); j++) {
                        auto ev = e->vals()->Get(j);
                        enumvals.push_back(VMEnumVal{ ev->name()->string_view(), ev->val() });
                    }
                }
                off = 0;
                for (flatbuffers::uoffset_t i = 0; i < bcf->enums()->size(); i++) {
                    auto e = bcf->enums()->Get(i);
                    auto fspan = span(enumvals.data() + off,
                                           enumvals.data() + off + e->vals()->size());
                    enums.push_back(VMEnum{ e->name()->string_view(), fspan, e->flags() });
                    off += e->vals()->size();
                }
                vector<int> ser_ids;
                for (flatbuffers::uoffset_t i = 0; i < bcf->ser_ids()->size(); i++) {
                    ser_ids.push_back(bcf->ser_ids()->Get(i));
                }
                vector<int> subfunctions_to_function;
                for (flatbuffers::uoffset_t i = 0; i < bcf->subfunctions_to_function()->size();
                     i++) {
                    subfunctions_to_function.push_back(bcf->subfunctions_to_function()->Get(i));
                }               
                VMMetaData vmmeta = {
                    bcf->metadata_version(),
                    span(type_table),
                    span(stringtable),
                    span(file_names),
                    span(function_names),
                    span(udts),
                    span(specidents),
                    span(enums),
                    span(ser_ids),
                    bcf->build_info()->string_view(),
                    bcf->src_hash(),
                    span(subfunctions_to_function),
                };
                auto vmargs = VMArgs {
                    nfr, string(fn), &vmmeta,
                    std::move(program_args),
                    (fun_base_t *)exports[1], (fun_base_t)exports[0], dump_leaks,
                    runtime_checks, stack_trace_python_ordering
                };
                lobster::VMAllocator vma(std::move(vmargs));
                vma.vm->EvalProgram();
                ret = vma.vm->evalret;
                return true;
            });
        if (!ok || !error.empty()) {
            // So we can see what the problem is..
            FILE *f = fopen((MainDir() + "compiled_lobster_jit_debug.c").c_str(), "w");
            if (f) {
                fputs(c_codegen.c_str(), f);
                fclose(f);
            }
            error = "libtcc JIT error: " + string(fn) + ":\n" + error;
        }
        return ret;
    #else
        (void)fn;
        (void)object_name;
        (void)program_args;
        (void)compile_only;
        (void)dump_leaks;
        (void)stack_trace_python_ordering;
        (void)c_codegen;
        (void)runtime_checks;
        (void)metadata_buffer;
        (void)nfr;
        error = "cannot JIT code: libtcc not enabled";
        return { "", 0 };
    #endif
}

Value CompileRun(VM &parent_vm, StackPtr &parent_sp, Value source, bool stringiscode,
                 vector<string> &&args) {
    string_view fn = stringiscode ? "string" : source.sval()->strv();  // fixme: datadir + sanitize?
    #ifdef USE_EXCEPTION_HANDLING
    try
    #endif
    {
        int runtime_checks = RUNTIME_ASSERT;  // FIXME: let caller decide?
        string metadata_buffer;
        string c_codegen;
        Compile(parent_vm.vma.nfr, fn, stringiscode ? source.sval()->strv() : string_view(),
                metadata_buffer, nullptr, nullptr, true, runtime_checks, nullptr, 1, false, true,
                c_codegen, false, "nullptr");
        string error;
        auto ret = RunTCC(parent_vm.vma.nfr, metadata_buffer, fn, nullptr, std::move(args),
                          false, error, runtime_checks, true, false, c_codegen);
        if (!error.empty()) THROW_OR_ABORT(error);
        Push(parent_sp, Value(parent_vm.NewString(ret.first)));
        return NilVal();
    }
    #ifdef USE_EXCEPTION_HANDLING
    catch (string &s) {
        Push(parent_sp, Value(parent_vm.NewString("nil")));
        return Value(parent_vm.NewString(s));
    }
    #endif
}

void AddCompiler(NativeRegistry &nfr) {  // it knows how to call itself!

nfr("compile_run_code", "code,args", "SS]", "SS?",
    "compiles and runs lobster source, sandboxed from the current program (in its own VM)."
    " the argument is a string of code. returns the return value of the program as a string,"
    " with an error string as second return value, or nil if none. using parse_data(),"
    " two program can communicate more complex data structures even if they don't have the same"
    " version of struct definitions.",
    [](StackPtr &sp, VM &vm, Value filename, Value args) {
        return CompileRun(vm, sp, filename, true, ValueToVectorOfStrings(args));
    });

nfr("compile_run_file", "filename,args", "SS]", "SS?",
    "same as compile_run_code(), only now you pass a filename.",
    [](StackPtr &sp, VM &vm, Value filename, Value args) {
        return CompileRun(vm, sp, filename, false, ValueToVectorOfStrings(args));
    });

}

void RegisterCoreLanguageBuiltins(NativeRegistry &nfr) {
    extern void AddBuiltins(NativeRegistry &nfr);
    extern void AddCompiler(NativeRegistry &nfr);
    extern void AddFile(NativeRegistry &nfr);
    extern void AddFlatBuffers(NativeRegistry &nfr);
    extern void AddReader(NativeRegistry &nfr);
    extern void AddMatrix(NativeRegistry &nfr);

    RegisterBuiltin(nfr, "", "builtin", AddBuiltins);
    RegisterBuiltin(nfr, "", "compiler", AddCompiler);
    RegisterBuiltin(nfr, "", "file", AddFile);
    RegisterBuiltin(nfr, "flatbuffers", "flatbuffers", AddFlatBuffers);
    RegisterBuiltin(nfr, "", "parsedata", AddReader);
    RegisterBuiltin(nfr, "matrix", "matrix", AddMatrix);
}

#if !LOBSTER_ENGINE
FileLoader EnginePreInit(NativeRegistry &nfr) {
    nfr.DoneRegistering();
    return DefaultLoadFile;
}
#endif

extern "C" int RunCompiledCodeMain(int argc, const char *const *argv, const VMMetaData *vmmeta,
                                   const lobster::fun_base_t *vtables,
                                   void *custom_pre_init, const char *aux_src_path) {
    #ifdef _MSC_VER
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        InitUnhandledExceptionFilter(argc, (char **)argv);
    #endif
    #ifdef USE_EXCEPTION_HANDLING
    try
    #endif
    {
        NativeRegistry nfr;
        RegisterCoreLanguageBuiltins(nfr);
        if (custom_pre_init) ((void (*)(NativeRegistry &))(custom_pre_init))(nfr);
        auto loader = EnginePreInit(nfr);
        min_output_level = OUTPUT_WARN;
        InitPlatform(GetMainDirFromExePath(argv[0]), aux_src_path, false, loader);
        auto from_lpak = true;
        uint64_t src_hash = 0;
        if (!LoadPakDir("default.lpak", src_hash)) {
            // FIXME: this is optional, we don't know if the compiled code wants to load this
            // file, so we don't error or even warn if this file can't be found.
            from_lpak = false;
        }
        auto vmargs = VMArgs {
            nfr,
            from_lpak ? string{} : StripDirPart(string_view_nt(argv[0])),
            vmmeta,
            {},
            vtables,
            nullptr,
            false,
            RUNTIME_ASSERT
        };
        for (int arg = 1; arg < argc; arg++) { vmargs.program_args.push_back(argv[arg]); }
        lobster::VMAllocator vma(std::move(vmargs));
        if (from_lpak && src_hash != vma.vm->vma.meta->src_hash) {
            THROW_OR_ABORT("lpak file from different version of the source code than the compiled code");
        }
        vma.vm->EvalProgram();
    }
    #ifdef USE_EXCEPTION_HANDLING
    catch (string &s) {
        LOG_ERROR(s);
        return 1;
    }
    #endif
    return 0;
}

bool Type::Equal(const Type &o, bool allow_unresolved) const {
    if (this == &o) return true;
    if (t != o.t) {
        if (!allow_unresolved) return false;
        // Special case for V_UUDT, since sometime types are resolved in odd orders.
        // TODO: can the IsGeneric() be removed?
        switch (t) {
            case V_UUDT:
                return IsUDT(o.t) && spec_udt->gudt == &o.udt->g && !spec_udt->IsGeneric();
            case V_CLASS:
            case V_STRUCT_R:
            case V_STRUCT_S:
                return o.t == V_UUDT && o.spec_udt->gudt == &udt->g && !o.spec_udt->IsGeneric();
            default:
                return false;
        }
    }
    if (sub == o.sub) return true;  // Also compares sf/udt
    switch (t) {
        case V_VECTOR:
        case V_NIL:
            return sub->Equal(*o.sub, allow_unresolved);
        case V_UUDT:
            return spec_udt->Equal(*o.spec_udt);
        default:
            return false;
    }
}

SubFunction::~SubFunction() { if (sbody) delete sbody; }

Field::~Field() { delete gdefaultval; }

Field::Field(const Field &o)
    : id(o.id),
      giventype(o.giventype),
      gdefaultval(o.gdefaultval ? o.gdefaultval->Clone(true) : nullptr),
      isprivate(o.isprivate),
      in_scope(o.in_scope),
      defined_in(o.defined_in) {}

UDT::~UDT() {
    for (auto &sfield : sfields) {
        delete sfield.defaultval;
    }
}

Function::~Function() {
    for (auto ov : overloads) delete ov;
    for (auto da : default_args) delete da;
}

Overload::~Overload() {
    delete gbody;
}

int Overload::NumSubf() {
    int sum = 0;
    for (auto csf = sf; csf; csf = csf->next)
        sum++;
    return sum;
}

bool SpecUDT::Equal(const SpecUDT &o) const {
    if (gudt != o.gudt ||
        IsGeneric() != o.IsGeneric() ||
        specializers.size() != o.specializers.size()) return false;
    for (auto [i, s] : enumerate(specializers)) {
        if (!s->Equal(*o.specializers[i])) return false;
    }
    return true;
}

bool SpecUDT::IsGeneric() const {
    assert(specializers.size() == gudt->generics.size());
    return !gudt->generics.empty();
}

}  // namespace lobster


stack_vector_storage g_svs1;
stack_vector_storage g_svs2;
stack_vector_storage g_svs4;
stack_vector_storage g_svs8;

#if STACK_PROFILING_ON
    vector<StackProfile> stack_profiles;
#endif
