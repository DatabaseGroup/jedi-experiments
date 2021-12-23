// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file src/exp-lookup.cc
///
/// \details
/// Implements an experimental environment that executes algorithms located in
/// the folder external/. The input file, the JEDI threshold, the output file, 
/// the query tree id, and the algorithms are passed as commandline arguments.
/// A quality and runtime evaluation of similarity lookup queries based on the 
/// JSON edit distance (JEDI) are computed.

#include <iostream>
#include <fstream>
#include <unistd.h>
#include "timing.h"
#include "node.h"
#include "json_label.h"
#include "unit_cost_model.h"
#include "label_intersection.h"
#include "bracket_notation_parser.h"
#include "tree_indexer.h"
#include "jedi_baseline_index.h"
#include "quickjedi_index.h"
#include "wang_index.h"
#include "jofilter_index.h"
#include "scan.h"
#include "index.h"
#include "two_stage_inverted_list.h"
#include "label_set_converter.h"
#include "label_set_element.h"
#include "lookup_result_element.h"

int main(int argc, char** argv) {
  using Label = label::JSONLabel;
  using CostModel = cost_model::UnitCostModelJSON<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  using TreeIndexer = node::TreeIndexJSON;
  using JEDIBASE = json::JEDIBaselineTreeIndex<CostModel, TreeIndexer>;
  using QUICKJEDI = json::QuickJEDITreeIndex<CostModel, TreeIndexer>;
  using WANG = json::WangTreeIndex<CostModel, TreeIndexer>;
  using JOFILTER = json::JOFilterTreeIndex<CostModel, TreeIndexer>;

  // Arguments needed for execution.
  if (argc != 6) {
    std::cout << "Please provide three input parameters:" << std::endl;
    std::cout << "(1) file_path, " << std::endl;
    std::cout << "(2) threshold, " << std::endl;
    std::cout << "(3) output file name, " << std::endl;
    std::cout << "(4) query_tree_id, " << std::endl;
    std::cout << "(5) algorithm selection [0 ... 18]." << std::endl;
    return -1;
  }

  // Command line argument indices.
  const int _COLLECTION = 1;
  const int _THRESHOLD = 2;
  const int _NAME = 3;
  const int _QUERY_ID = 4;
  const int _ALGORITHMS = 5;

  // Initialize label dictionary.
  LabelDictionary ld;
  
  // Initialize cost model.
  CostModel ucm(ld);
  JEDIBASE baseline_algorithm(ucm);
  QUICKJEDI quickjedi_algorithm(ucm);
  WANG wang_algorithm(ucm);
  JOFILTER jofilter_algorithm(ucm);

  // Data storage.
  std::vector<lookup::LookupResultElement> scan_baseline;
  std::vector<lookup::LookupResultElement> scan_quickjedi;
  std::vector<lookup::LookupResultElement> scan_baseline_wang;
  std::vector<lookup::LookupResultElement> scan_quickjedi_wang;
  std::vector<lookup::LookupResultElement> scan_baseline_jofilter;
  std::vector<lookup::LookupResultElement> scan_quickjedi_jofilter;
  std::vector<lookup::LookupResultElement> index_baseline;
  std::vector<lookup::LookupResultElement> index_quickjedi;
  std::vector<lookup::LookupResultElement> index_baseline_wang;
  std::vector<lookup::LookupResultElement> index_quickjedi_wang;
  std::vector<lookup::LookupResultElement> index_baseline_jofilter;
  std::vector<lookup::LookupResultElement> index_quickjedi_jofilter;
  long long int scan_baseline_ver = 0;
  long long int scan_quickjedi_ver = 0;
  long long int scan_baseline_wang_ver = 0;
  long long int scan_quickjedi_wang_ver = 0;
  long long int scan_baseline_jofilter_ver = 0;
  long long int scan_quickjedi_jofilter_ver = 0;
  long long int index_baseline_ver = 0;
  long long int index_quickjedi_ver = 0;
  long long int index_baseline_wang_ver = 0;
  long long int index_quickjedi_wang_ver = 0;
  long long int index_baseline_jofilter_ver = 0;
  long long int index_quickjedi_jofilter_ver = 0;
  long long int scan_baseline_ub = 0;
  long long int scan_quickjedi_ub = 0;
  long long int scan_baseline_wang_ub = 0;
  long long int scan_quickjedi_wang_ub = 0;
  long long int scan_baseline_jofilter_ub = 0;
  long long int scan_quickjedi_jofilter_ub = 0;
  long long int index_baseline_ub = 0;
  long long int index_quickjedi_ub = 0;
  long long int index_baseline_wang_ub = 0;
  long long int index_quickjedi_wang_ub = 0;
  long long int index_baseline_jofilter_ub = 0;
  long long int index_quickjedi_jofilter_ub = 0;
  long long int scan_baseline_cand = 0;
  long long int scan_quickjedi_cand = 0;
  long long int scan_baseline_wang_cand = 0;
  long long int scan_quickjedi_wang_cand = 0;
  long long int scan_baseline_jofilter_cand = 0;
  long long int scan_quickjedi_jofilter_cand = 0;
  long long int index_baseline_cand = 0;
  long long int index_quickjedi_cand = 0;
  long long int index_baseline_wang_cand = 0;
  long long int index_quickjedi_wang_cand = 0;
  long long int index_baseline_jofilter_cand = 0;
  long long int index_quickjedi_jofilter_cand = 0;
  long long int scan_baseline_pre_cand = 0;
  long long int scan_quickjedi_pre_cand = 0;
  long long int scan_baseline_wang_pre_cand = 0;
  long long int scan_quickjedi_wang_pre_cand = 0;
  long long int scan_baseline_jofilter_pre_cand = 0;
  long long int scan_quickjedi_jofilter_pre_cand = 0;
  long long int index_baseline_pre_cand = 0;
  long long int index_quickjedi_pre_cand = 0;
  long long int index_baseline_wang_pre_cand = 0;
  long long int index_quickjedi_wang_pre_cand = 0;
  long long int index_baseline_jofilter_pre_cand = 0;
  long long int index_quickjedi_jofilter_pre_cand = 0;

  // Path to file containing the input tree.
  std::string file_path = argv[_COLLECTION];

  // Set distance threshold - maximum number of allowed edit operations.
  double distance_threshold = std::stod(argv[_THRESHOLD]);

  // Output file name.
  std::string outfile_name = argv[_NAME];

  // Query tree id in collection.
  unsigned int query_tree_id = std::stoi(argv[_QUERY_ID]);

  // Algorithm paramters: 
  int algorithms = std::stoi(argv[_ALGORITHMS]);

  // Timing object for runtime measurements.
  Timing timing;

  // Output stream to write the results.
  std::ofstream runtime_file;
  std::ofstream meta_file;
  std::ofstream quality_file;

  // Create runtime results file.
  runtime_file.open(outfile_name + "_" + std::to_string(distance_threshold) + 
      "_" + std::to_string(query_tree_id) + "_" + "runtime.txt");
  runtime_file << "scan_baseline,scan_baseline,scan_baseline_wang," <<
      "scan_baseline_wang,scan_baseline_wang,scan_baseline_wang," <<
      "index_baseline,index_baseline,index_baseline_wang,index_baseline_wang," <<
      "index_baseline_wang,index_baseline_wang\n";
  runtime_file.close();
  
  // Create quality results file.
  quality_file.open (outfile_name + "_" + std::to_string(distance_threshold) + 
      "_" + std::to_string(query_tree_id) + "_" + "quality.txt");
  quality_file << "T1_ID,T2_ID,T1_SIZE,T1_SIZE,LOWERBOUND,UPPERBOUND,JEDI\n";
  quality_file.close();

  //////////////////////////////////////////////////////////////////////////////
  // Parsing input trees.
  std::cout << " == PARSING == " << file_path << std::endl;
  auto* parsing_t = timing.create_enroll("Parsing");
  parsing_t->start();

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  // Parse given input collection.
  parser::BracketNotationParser<Label> bnp;
  bnp.parse_collection(trees_collection, file_path);
  long int collection_size = trees_collection.size();

  parsing_t->stop();

  //////////////////////////////////////////////////////////////////////////////
  // CONVERT TREES TO LABEL SETS.
  std::vector<std::pair<int, std::vector<label_set_converter_index::LabelSetElement>>> sets_collection;
  std::vector<std::pair<int, int>> size_setid_map;
  label_set_converter_index::Converter<Label> lsc;
  lsc.assignFrequencyIdentifiers(trees_collection, sets_collection, size_setid_map);
  unsigned int label_cnt = lsc.get_number_of_labels();

  // Create meta data file.
  meta_file.open (outfile_name + "_" + std::to_string(distance_threshold) + 
      "_" + std::to_string(query_tree_id) + "_" + "meta.txt");
  meta_file << "COLSIZE,QUERYTREEID,PARSINGTIME,LABELUNIVERSESIZE,THRESHOLD\n";
  meta_file << trees_collection.size() << "," << 
      trees_collection[query_tree_id].get_tree_size() << "," << 
      parsing_t->getfloat() << "," << label_cnt << "," << 
      distance_threshold << "\n";
  meta_file.close();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH JEDIBASE VERIFICATION.
  std::cout << " == SCAN WITH JEDIBASE VERIFICATION == " << std::endl;
  auto* scan_baseline_t = timing.create_enroll("Scan; JEDIBASE; No UB");

  // Initialize lookup algorithm.
  lookup::VerificationScan<Label, JEDIBASE> sd;

  scan_baseline_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 2 || algorithms == 3) {
    scan_baseline = sd.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_baseline_t->stop();
  scan_baseline_pre_cand = collection_size;
  scan_baseline_cand = collection_size;
  scan_baseline_ub = scan_baseline_cand - sd.get_verification_count();
  scan_baseline_ver = sd.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH QUICKJEDI VERIFICATION.
  std::cout << " == SCAN WITH QUICKJEDI VERIFICATION == " << std::endl;
  auto* scan_quickjedi_t = timing.create_enroll("Scan; QUICKJEDI; No UB");

  // Initialize lookup algorithm.
  lookup::VerificationScan<Label, QUICKJEDI> ss;

  scan_quickjedi_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 6 || algorithms == 7) {
    scan_quickjedi = ss.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_quickjedi_t->stop();
  scan_quickjedi_pre_cand = collection_size;
  scan_quickjedi_cand = collection_size;
  scan_quickjedi_ub = scan_quickjedi_cand - ss.get_verification_count();
  scan_quickjedi_ver = ss.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH JEDIBASE VERIFICATION AND WANG UPPER BOUND.
  std::cout << " == SCAN WITH JEDIBASE VERIFICATION AND WANG UPPER BOUND == " << std::endl;
  auto* scan_baseline_wang_t = timing.create_enroll("Scan; JEDIBASE; WANG UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkScan<Label, JEDIBASE, JOFILTER> sdo;

  scan_baseline_wang_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 2 || algorithms == 4) {
    scan_baseline_wang = sdo.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_baseline_wang_t->stop();
  scan_baseline_wang_pre_cand = collection_size;
  scan_baseline_wang_cand = collection_size;
  scan_baseline_wang_ub = scan_baseline_wang_cand - sdo.get_verification_count();
  scan_baseline_wang_ver = sdo.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH QUICKJEDI VERIFICATION AND WANG UPPER BOUND.
  std::cout << " == SCAN WITH QUICKJEDI VERIFICATION AND WANG UPPER BOUND == " << std::endl;
  auto* scan_quickjedi_wang_t = timing.create_enroll("Scan; QUICKJEDI; WANG UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkScan<Label, QUICKJEDI, JOFILTER> sso;

  scan_quickjedi_wang_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 6 || algorithms == 8) {
    scan_quickjedi_wang = sso.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_quickjedi_wang_t->stop();
  scan_quickjedi_wang_pre_cand = collection_size;
  scan_quickjedi_wang_cand = collection_size;
  scan_quickjedi_wang_ub = scan_quickjedi_wang_cand - sso.get_verification_count();
  scan_quickjedi_wang_ver = sso.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH JEDIBASE VERIFICATION AND JOFILTER UPPER BOUND.
  std::cout << " == SCAN WITH JEDIBASE VERIFICATION AND JOFILTER UPPER BOUND == " << std::endl;
  auto* scan_baseline_jofilter_t = timing.create_enroll("Scan; JEDIBASE; JOFILTER UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkScan<Label, JEDIBASE, JOFILTER> sdok;

  scan_baseline_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 2 || algorithms == 5) {
    scan_baseline_jofilter = sdok.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_baseline_jofilter_t->stop();
  scan_baseline_jofilter_pre_cand = collection_size;
  scan_baseline_jofilter_cand = collection_size;
  scan_baseline_jofilter_ub = scan_baseline_jofilter_cand - sdok.get_verification_count();
  scan_baseline_jofilter_ver = sdok.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN WITH QUICKJEDI VERIFICATION AND JOFILTER UPPER BOUND.
  std::cout << " == SCAN WITH QUICKJEDI VERIFICATION AND JOFILTER UPPER BOUND == " << std::endl;
  auto* scan_quickjedi_jofilter_t = timing.create_enroll("Scan; QUICKJEDI; JOFILTER UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkScan<Label, QUICKJEDI, JOFILTER> ssok;

  scan_quickjedi_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 6 || algorithms == 9) {
    scan_quickjedi_jofilter = ssok.execute_lookup(trees_collection, query_tree_id, 
        distance_threshold);
  }
  scan_quickjedi_jofilter_t->stop();
  scan_quickjedi_jofilter_pre_cand = collection_size;
  scan_quickjedi_jofilter_cand = collection_size;
  scan_quickjedi_jofilter_ub = scan_quickjedi_jofilter_cand - ssok.get_verification_count();
  scan_quickjedi_jofilter_ver = ssok.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // BUILD INDEX.
  lookup::TwoStageInvertedList tsil(label_cnt);
  tsil.build(sets_collection);

  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH JEDIBASE VERIFICATION.
  std::cout << " == INDEX SCAN WITH JEDIBASE VERIFICATION == " << std::endl;
  auto* index_baseline_t = timing.create_enroll("Index Scan; JEDIBASE; No UB");

  // Initialize lookup algorithm.
  lookup::VerificationIndex<Label, JEDIBASE> id;

  index_baseline_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 11 || 
      algorithms == 12) {
    index_baseline = id.execute_lookup(trees_collection, sets_collection, 
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_baseline_t->stop();
  index_baseline_pre_cand = id.get_pre_candidates_count();
  index_baseline_cand = id.get_candidates_count();
  index_baseline_ub = index_baseline_cand - id.get_verification_count();
  index_baseline_ver = id.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH QUICKJEDI VERIFICATION.
  std::cout << " == INDEX SCAN WITH QUICKJEDI VERIFICATION == " << std::endl;
  auto* index_quickjedi_t = timing.create_enroll("Index Scan; QUICKJEDI; No UB");

  // Initialize lookup algorithm.
  lookup::VerificationIndex<Label, QUICKJEDI> is;

  index_quickjedi_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 15 || 
      algorithms == 16 || algorithms == 19) {
    index_quickjedi = is.execute_lookup(trees_collection, sets_collection, 
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_quickjedi_t->stop();
  index_quickjedi_pre_cand = is.get_pre_candidates_count();
  index_quickjedi_cand = is.get_candidates_count();
  index_quickjedi_ub = index_quickjedi_cand - is.get_verification_count();
  index_quickjedi_ver = is.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH JEDIBASE VERIFICATION AND WANG UPPER BOUND.
  std::cout << " == INDEX SCAN WITH JEDIBASE VERIFICATION AND WANG UPPER BOUND == " << std::endl;
  auto* index_baseline_wang_t = timing.create_enroll("Index Scan; JEDIBASE; WANG UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, JEDIBASE, JOFILTER> ido;

  index_baseline_wang_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 11 || 
      algorithms == 13) {
    index_baseline_wang = ido.execute_lookup(trees_collection, sets_collection, 
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_baseline_wang_t->stop();
  index_baseline_wang_pre_cand = ido.get_pre_candidates_count();
  index_baseline_wang_cand = ido.get_candidates_count();
  index_baseline_wang_ub = index_baseline_wang_cand - ido.get_verification_count();
  index_baseline_wang_ver = ido.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH QUICKJEDI VERIFICATION AND WANG UPPER BOUND.
  std::cout << " == INDEX SCAN WITH QUICKJEDI VERIFICATION AND WANG UPPER BOUND == " << std::endl;
  auto* index_quickjedi_wang_t = timing.create_enroll("Index Scan; QUICKJEDI; WANG UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, QUICKJEDI, JOFILTER> iso;

  index_quickjedi_wang_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 15 || 
      algorithms == 17 || algorithms == 19 || algorithms == 20) {
    index_quickjedi_wang = iso.execute_lookup(trees_collection, sets_collection, 
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_quickjedi_wang_t->stop();
  index_quickjedi_wang_pre_cand = iso.get_pre_candidates_count();
  index_quickjedi_wang_cand = iso.get_candidates_count();
  index_quickjedi_wang_ub = index_quickjedi_wang_cand - iso.get_verification_count();
  index_quickjedi_wang_ver = iso.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH JEDIBASE VERIFICATION AND JOFILTER UPPER BOUND.
  std::cout << " == INDEX SCAN WITH JEDIBASE VERIFICATION AND JOFILTER UPPER BOUND == " << std::endl;
  auto* index_baseline_jofilter_t = timing.create_enroll("Index Scan; JEDIBASE; JOFILTER UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, JEDIBASE, JOFILTER> idok;

  index_baseline_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 11 || 
      algorithms == 14 || algorithms == 19 || algorithms == 20) {
    index_baseline_jofilter = idok.execute_lookup(trees_collection, 
      sets_collection, size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_baseline_jofilter_t->stop();
  index_baseline_jofilter_pre_cand = idok.get_pre_candidates_count();
  index_baseline_jofilter_cand = idok.get_candidates_count();
  index_baseline_jofilter_ub = index_baseline_jofilter_cand - idok.get_verification_count();
  index_baseline_jofilter_ver = idok.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // INDEX SCAN WITH QUICKJEDI VERIFICATION AND JOFILTER UPPER BOUND.
  std::cout << " == INDEX SCAN WITH QUICKJEDI VERIFICATION AND JOFILTER UPPER BOUND == " << std::endl;
  auto* index_quickjedi_jofilter_t = timing.create_enroll("Index Scan; QUICKJEDI; JOFILTER UB");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, QUICKJEDI, JOFILTER> isok;

  index_quickjedi_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 10 || algorithms == 15 || 
      algorithms == 18 || algorithms == 19 || algorithms == 20) {
    index_quickjedi_jofilter = isok.execute_lookup(trees_collection, 
      sets_collection, size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  index_quickjedi_jofilter_t->stop();
  index_quickjedi_jofilter_pre_cand = isok.get_pre_candidates_count();
  index_quickjedi_jofilter_cand = isok.get_candidates_count();
  index_quickjedi_jofilter_ub = index_quickjedi_jofilter_cand - isok.get_verification_count();
  index_quickjedi_jofilter_ver = isok.get_verification_count();


  // //////////////////////////////////////////////////////////////////////////////
  // Print the results.


  runtime_file.open (outfile_name + "_" + std::to_string(distance_threshold) + "_" + 
      std::to_string(query_tree_id) + "_" + "runtime.txt", std::ios_base::app);
  runtime_file << //"#PRE_CANDIDATES," << 
  scan_baseline_pre_cand << "," << scan_quickjedi_pre_cand << "," << 
  scan_baseline_wang_pre_cand << "," << scan_quickjedi_wang_pre_cand << "," << 
  scan_baseline_jofilter_pre_cand << "," << scan_quickjedi_jofilter_pre_cand << "," << 
  index_baseline_pre_cand << "," << index_quickjedi_pre_cand << "," << 
  index_baseline_wang_pre_cand << "," << index_quickjedi_wang_pre_cand << "," << 
  index_baseline_jofilter_pre_cand << "," << index_quickjedi_jofilter_pre_cand << "\n";
  runtime_file << //"#CANDIDATES," << 
  scan_baseline_cand << "," << scan_quickjedi_cand << "," << 
  scan_baseline_wang_cand << "," << scan_quickjedi_wang_cand << "," << 
  scan_baseline_jofilter_cand << "," << scan_quickjedi_jofilter_cand << "," << 
  index_baseline_cand << "," << index_quickjedi_cand << "," << 
  index_baseline_wang_cand << "," << index_quickjedi_wang_cand << "," << 
  index_baseline_jofilter_cand << "," << index_quickjedi_jofilter_cand << "\n";
  runtime_file << //"#UPPER BOUND," << 
  scan_baseline_ub << "," << scan_quickjedi_ub << "," << 
  scan_baseline_wang_ub << "," << scan_quickjedi_wang_ub << "," << 
  scan_baseline_jofilter_ub << "," << scan_quickjedi_jofilter_ub << "," << 
  index_baseline_ub << "," << index_quickjedi_ub << "," << 
  index_baseline_wang_ub << "," << index_quickjedi_wang_ub << "," << 
  index_baseline_jofilter_ub << "," << index_quickjedi_jofilter_ub << "\n";
  runtime_file << //"#VERIFICATION," << 
  scan_baseline_ver << "," << scan_quickjedi_ver << "," << 
  scan_baseline_wang_ver << "," << scan_quickjedi_wang_ver << "," << 
  scan_baseline_jofilter_ver << "," << scan_quickjedi_jofilter_ver << "," << 
  index_baseline_ver << "," << index_quickjedi_ver << "," << 
  index_baseline_wang_ver << "," << index_quickjedi_wang_ver << "," << 
  index_baseline_jofilter_ver << "," << index_quickjedi_jofilter_ver << "\n";
  runtime_file << //"RES_SIZE," << 
  scan_baseline.size() << "," << scan_quickjedi.size() << "," << 
  scan_baseline_wang.size() << "," << scan_quickjedi_wang.size() << "," << 
  scan_baseline_jofilter.size() << "," << scan_quickjedi_jofilter.size() << "," << 
  index_baseline.size() << "," << index_quickjedi.size() << "," << 
  index_baseline_wang.size() << "," << index_quickjedi_wang.size() << "," << 
  index_baseline_jofilter.size() << "," << index_quickjedi_jofilter.size() << "\n";
  runtime_file << //"EXE_TIME," << 
  scan_baseline_t->getfloat() << "," << scan_quickjedi_t->getfloat() << "," << 
  scan_baseline_wang_t->getfloat() << "," << scan_quickjedi_wang_t->getfloat() << "," << 
  scan_baseline_jofilter_t->getfloat() << "," << scan_quickjedi_jofilter_t->getfloat() << "," << 
  index_baseline_t->getfloat() << "," << index_quickjedi_t->getfloat() << "," << 
  index_baseline_wang_t->getfloat() << "," << index_quickjedi_wang_t->getfloat() << "," << 
  index_baseline_jofilter_t->getfloat() << "," << index_quickjedi_jofilter_t->getfloat() << "\n";
  runtime_file.close();
  usleep(100000); // Wait 100ms for writing to disk.

  // Print quality measures.
  quality_file.open (outfile_name + "_" + std::to_string(distance_threshold) + "_" + std::to_string(query_tree_id) + "_" + "quality.txt", std::ios_base::app);
  for (auto& result_pair: index_quickjedi_jofilter) {
    quality_file << result_pair.tree_id_1 << "," << result_pair.tree_id_2 << "," << 
    trees_collection[result_pair.tree_id_1].get_tree_size() << "," << 
    trees_collection[result_pair.tree_id_2].get_tree_size() << "," <<
    result_pair.lower_bound << "," << result_pair.upper_bound << "," <<
    result_pair.jedi_value << "\n";
  }
  quality_file.close();
  usleep(100000); // Wait 100ms for writing to disk.

  return 0;
}
