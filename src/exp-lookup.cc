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
  using LabelSetElem = label_set_converter_index::LabelSetElement;
  using CostModel = cost_model::UnitCostModelJSON<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  using TreeIndexer = node::TreeIndexJSON;
  using JEDIBASE = json::JEDIBaselineTreeIndex<CostModel, TreeIndexer>;
  using QUICKJEDI = json::QuickJEDITreeIndex<CostModel, TreeIndexer>;
  using WANG = json::WangTreeIndex<CostModel, TreeIndexer>;
  using JOFILTER = json::JOFilterTreeIndex<CostModel, TreeIndexer>;

  // Arguments needed for execution.
  if (argc != 6) {
    std::cout << "Please provide five input parameters:" << std::endl;
    std::cout << "(1) file_path, " << std::endl;
    std::cout << "(2) threshold, " << std::endl;
    std::cout << "(3) output file name, " << std::endl;
    std::cout << "(4) query_tree_id, " << std::endl;
    std::cout << "(5) algorithm selection [0 ... 5]:" << std::endl;
    std::cout << "  - 0: all algorithms." << std::endl;
    std::cout << "  - 1: all algorithms using JSIM." << std::endl;
    std::cout << "  - 2: all algorithms using JSIM, QuickJedi." << std::endl;
    std::cout << "  - 3: all algorithms using JSIM, JediOrder." << std::endl;
    std::cout << "  - 4: all algorithms using JSIM, QuickJedi|JediOrder." << 
      std::endl;
    std::cout << "  - 5: algorithm that uses JSIM, QuickJedi, JOFilter." << 
      std::endl;
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
  std::vector<lookup::LookupResultElement> scan_quickjedi_jofilter;
  std::vector<lookup::LookupResultElement> jsim_baseline;
  std::vector<lookup::LookupResultElement> jsim_quickjedi;
  std::vector<lookup::LookupResultElement> jsim_baseline_wang;
  std::vector<lookup::LookupResultElement> jsim_quickjedi_jofilter;
  long long int scan_quickjedi_jofilter_ver = 0;
  long long int jsim_baseline_ver = 0;
  long long int jsim_quickjedi_ver = 0;
  long long int jsim_baseline_wang_ver = 0;
  long long int jsim_quickjedi_jofilter_ver = 0;
  long long int scan_quickjedi_jofilter_ub = 0;
  long long int jsim_baseline_ub = 0;
  long long int jsim_quickjedi_ub = 0;
  long long int jsim_baseline_wang_ub = 0;
  long long int jsim_quickjedi_jofilter_ub = 0;
  long long int scan_quickjedi_jofilter_cand = 0;
  long long int jsim_baseline_cand = 0;
  long long int jsim_quickjedi_cand = 0;
  long long int jsim_baseline_wang_cand = 0;
  long long int jsim_quickjedi_jofilter_cand = 0;
  long long int scan_quickjedi_jofilter_pre_cand = 0;
  long long int jsim_baseline_pre_cand = 0;
  long long int jsim_quickjedi_pre_cand = 0;
  long long int jsim_baseline_wang_pre_cand = 0;
  long long int jsim_quickjedi_jofilter_pre_cand = 0;

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

  // Verify whether algorithms parameter is set correctly.
  if (algorithms < 0 || algorithms > 5) {
    std::cout << "Error: incorrect algorithms parameter." << std::endl;
    std::cout << std::endl;
    std::cout << "Please provide five input parameters:" << std::endl;
    std::cout << "(1) file_path, " << std::endl;
    std::cout << "(2) threshold, " << std::endl;
    std::cout << "(3) output file name, " << std::endl;
    std::cout << "(4) query_tree_id, " << std::endl;
    std::cout << "(5) algorithm selection [0 ... 5]:" << std::endl;
    std::cout << "  - 0: all algorithms." << std::endl;
    std::cout << "  - 1: all algorithms using JSIM." << std::endl;
    std::cout << "  - 2: all algorithms using JSIM, QuickJedi." << std::endl;
    std::cout << "  - 3: all algorithms using JSIM, JediOrder." << std::endl;
    std::cout << "  - 4: all algorithms using JSIM, QuickJedi|JediOrder." << 
      std::endl;
    std::cout << "  - 5: algorithm that uses JSIM, QuickJedi, JOFilter." << 
      std::endl;
    return -1;
  }

  // Timing object for runtime measurements.
  Timing timing;

  // Output stream to write the results.
  std::ofstream runtime_file;
  std::ofstream meta_file;
  std::ofstream quality_file;

  // Create runtime results file.
  runtime_file.open(outfile_name + "-" + std::to_string(distance_threshold) +
      "-" + std::to_string(query_tree_id) + "-" + "runtime.txt");
  runtime_file << "scan-quickjedi-jofilter,jsim-baseline,jsim-quickjedi," <<
      "jsim-baseline_wang,jsim-quickjedi-jofilter\n";
  runtime_file.close();
 
  // Create quality results file.
  quality_file.open (outfile_name + "-" + std::to_string(distance_threshold) +
      "-" + std::to_string(query_tree_id) + "-" + "quality.txt");
  quality_file << "T1-ID,T2-ID,T1-SIZE,T1-SIZE,LOWERBOUND,UPPERBOUND,JEDI\n";
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
  std::vector<std::pair<int, std::vector<LabelSetElem>>> sets_collection;
  std::vector<std::pair<int, int>> size_setid_map;
  label_set_converter_index::Converter<Label> lsc;
  lsc.assignFrequencyIdentifiers(trees_collection, sets_collection, 
    size_setid_map);
  unsigned int label_cnt = lsc.get_number_of_labels();

  // Create meta data file.
  meta_file.open (outfile_name + "-" + std::to_string(distance_threshold) +
      "-" + std::to_string(query_tree_id) + "-" + "meta.txt");
  meta_file << "COLSIZE,QUERYTREEID,PARSINGTIME,LABELUNIVERSESIZE,THRESHOLD\n";
  meta_file << trees_collection.size() << "," <<
      trees_collection[query_tree_id].get_tree_size() << "," <<
      parsing_t->getfloat() << "," << label_cnt << "," <<
      distance_threshold << "\n";
  meta_file.close();

  //////////////////////////////////////////////////////////////////////////////
  // SCAN, QUICKJEDI, JOFILTER
  std::cout << " == SCAN, QUICKJEDI, JOFILTER == " << std::endl;
  auto* scan_quickjedi_jofilter_t = 
    timing.create_enroll("SCAN, QUICKJEDI, JOFILTER");

  // Initialize lookup algorithm.
  lookup::VerificationUBkScan<Label, QUICKJEDI, JOFILTER> ssok;

  scan_quickjedi_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0) {
    scan_quickjedi_jofilter = ssok.execute_lookup(trees_collection,
        query_tree_id, distance_threshold);
  }
  scan_quickjedi_jofilter_t->stop();
  scan_quickjedi_jofilter_pre_cand = collection_size;
  scan_quickjedi_jofilter_cand = collection_size;
  scan_quickjedi_jofilter_ub = scan_quickjedi_jofilter_cand - 
    ssok.get_verification_count();
  scan_quickjedi_jofilter_ver = ssok.get_verification_count();

  //////////////////////////////////////////////////////////////////////////////
  // BUILD JSIM INDEX.
  lookup::TwoStageInvertedList tsil(label_cnt);
  tsil.build(sets_collection);

  //////////////////////////////////////////////////////////////////////////////
  // JSIM, BASELINE.
  std::cout << " == JSIM, BASELINE == " << std::endl;
  auto* jsim_baseline_t = timing.create_enroll("JSIM, BASELINE");

  // Initialize lookup algorithm.
  lookup::VerificationIndex<Label, JEDIBASE> id;

  jsim_baseline_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1) {
    jsim_baseline = id.execute_lookup(trees_collection, sets_collection,
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  jsim_baseline_t->stop();
  jsim_baseline_pre_cand = id.get_pre_candidates_count();
  jsim_baseline_cand = id.get_candidates_count();
  jsim_baseline_ub = jsim_baseline_cand - id.get_verification_count();
  jsim_baseline_ver = id.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // JSIM, QUICKJEDI.
  std::cout << " == JSIM, QUICKJEDI == " << std::endl;
  auto* jsim_quickjedi_t = timing.create_enroll("JSIM, QUICKJEDI");

  // Initialize lookup algorithm.
  lookup::VerificationIndex<Label, QUICKJEDI> is;

  jsim_quickjedi_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 2 ||
      algorithms == 4) {
    jsim_quickjedi = is.execute_lookup(trees_collection, sets_collection,
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  jsim_quickjedi_t->stop();
  jsim_quickjedi_pre_cand = is.get_pre_candidates_count();
  jsim_quickjedi_cand = is.get_candidates_count();
  jsim_quickjedi_ub = jsim_quickjedi_cand - is.get_verification_count();
  jsim_quickjedi_ver = is.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // JSIM, BASELINE, WANG.
  std::cout << " == JSIM, BASELINE, WANG == " << std::endl;
  auto* jsim_baseline_wang_t = timing.create_enroll("JSIM, BASELINE, WANG");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, JEDIBASE, JOFILTER> ido;

  jsim_baseline_wang_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 3 ||
      algorithms == 4) {
    jsim_baseline_wang = ido.execute_lookup(trees_collection, sets_collection,
        size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  jsim_baseline_wang_t->stop();
  jsim_baseline_wang_pre_cand = ido.get_pre_candidates_count();
  jsim_baseline_wang_cand = ido.get_candidates_count();
  jsim_baseline_wang_ub = jsim_baseline_wang_cand - 
    ido.get_verification_count();
  jsim_baseline_wang_ver = ido.get_verification_count();


  //////////////////////////////////////////////////////////////////////////////
  // JSIM, QUICKJEDI, JOFILTER.
  std::cout << " == JSIM, QUICKJEDI, JOFILTER == " << std::endl;
  auto* jsim_quickjedi_jofilter_t = 
    timing.create_enroll("JSIM, QUICKJEDI, JOFILTER");

  // Initialize lookup algorithm.
  lookup::VerificationUBkIndex<Label, QUICKJEDI, JOFILTER> isok;

  jsim_quickjedi_jofilter_t->start();
  // Verify all computed lookup candidates and return the lookup result.
  if (algorithms == 0 || algorithms == 1 || algorithms == 2 ||
      algorithms == 3 || algorithms == 4 || algorithms == 5) {
    jsim_quickjedi_jofilter = isok.execute_lookup(trees_collection,
      sets_collection, size_setid_map, tsil, query_tree_id, distance_threshold);
  }
  jsim_quickjedi_jofilter_t->stop();
  jsim_quickjedi_jofilter_pre_cand = isok.get_pre_candidates_count();
  jsim_quickjedi_jofilter_cand = isok.get_candidates_count();
  jsim_quickjedi_jofilter_ub = jsim_quickjedi_jofilter_cand - 
    isok.get_verification_count();
  jsim_quickjedi_jofilter_ver = isok.get_verification_count();


  // //////////////////////////////////////////////////////////////////////////////
  // Print the results.


  runtime_file.open (outfile_name + "-" + std::to_string(distance_threshold) + 
    "-" + 
    std::to_string(query_tree_id) + "-" + "runtime.txt", std::ios_base::app);
  runtime_file << //"#PRE_CANDIDATES," <<
  scan_quickjedi_jofilter_pre_cand << "," << jsim_baseline_pre_cand << "," << 
  jsim_quickjedi_pre_cand << "," << jsim_baseline_wang_pre_cand << "," << 
  jsim_quickjedi_jofilter_pre_cand << "\n";
  runtime_file << //"#CANDIDATES," <<
  scan_quickjedi_jofilter_cand << "," << jsim_baseline_cand << "," << 
  jsim_quickjedi_cand << "," << jsim_baseline_wang_cand << "," <<
  jsim_quickjedi_jofilter_cand << "\n";
  runtime_file << //"#UPPER BOUND," <<
  scan_quickjedi_jofilter_ub << "," << jsim_baseline_ub << "," << 
  jsim_quickjedi_ub << "," << jsim_baseline_wang_ub << "," <<
  jsim_quickjedi_jofilter_ub << "\n";
  runtime_file << //"#VERIFICATION," <<
  scan_quickjedi_jofilter_ver << "," << jsim_baseline_ver << "," << 
  jsim_quickjedi_ver << "," << jsim_baseline_wang_ver << "," << 
  jsim_quickjedi_jofilter_ver << "\n";
  runtime_file << //"RES_SIZE," <<
  scan_quickjedi_jofilter.size() << "," << jsim_baseline.size() << "," << 
  jsim_quickjedi.size() << "," << jsim_baseline_wang.size() << "," << 
  jsim_quickjedi_jofilter.size() << "\n";
  runtime_file << //"EXE_TIME," <<
  scan_quickjedi_jofilter_t->getfloat() << "," << 
  jsim_baseline_t->getfloat() << "," << jsim_quickjedi_t->getfloat() << "," <<
  jsim_baseline_wang_t->getfloat() << "," << 
  jsim_quickjedi_jofilter_t->getfloat() << "\n";
  runtime_file.close();
  usleep(100000); // Wait 100ms for writing to disk.

  // Print quality measures.
  quality_file.open (outfile_name + "-" + std::to_string(distance_threshold) +
    "-" + 
    std::to_string(query_tree_id) + "-" + "quality.txt", std::ios_base::app);
  for (auto& result_pair: jsim_quickjedi_jofilter) {
    quality_file << result_pair.tree_id_1 << "," << result_pair.tree_id_2 << 
    "," << trees_collection[result_pair.tree_id_1].get_tree_size() << "," <<
    trees_collection[result_pair.tree_id_2].get_tree_size() << "," <<
    result_pair.lower_bound << "," << result_pair.upper_bound << "," <<
    result_pair.jedi_value << "\n";
  }
  quality_file.close();
  usleep(100000); // Wait 100ms for writing to disk.

  return 0;
}
