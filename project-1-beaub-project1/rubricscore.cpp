
///////////////////////////////////////////////////////////////////////////////
// rubricscore.cpp
//
// Program that cross-references googletest XML output against a scoring
// rubric in JSON, and prints out a grade score based on how many tests
// passsed.
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

class parse_exception : public std::exception {
private:
  std::string what_;

public:

  parse_exception(const std::string& what)
  : what_(what) { }

  virtual const char* what() const noexcept { return what_.c_str(); }
};

class testsuite {
private:
  std::string name_;
  unsigned tests_, failures_, disabled_, errors_, time_;

public:

  testsuite(std::string&& name,
            unsigned tests,
            unsigned failures,
            unsigned disabled,
            unsigned errors,
            unsigned time)
  : name_(name), tests_(tests), failures_(failures), disabled_(disabled),
    errors_(errors), time_(time) { }

  testsuite()
  : testsuite(std::string(), 0, 0, 0, 0, 0) { }

  const std::string& name() const { return name_; }
  unsigned tests() const { return tests_; }
  unsigned failures() const { return failures_; }
  unsigned disabled() const { return disabled_; }
  unsigned errors() const { return errors_; }
  unsigned time() const { return time_; }
};

using test_results = std::map<std::string, testsuite>;

// throws parse_exception on I/O or parse error
test_results load_test_results(const std::string& googletest_xml_path) {

  boost::property_tree::ptree tree;
  try {
    boost::property_tree::read_xml(googletest_xml_path, tree);
  } catch (boost::property_tree::xml_parser_error e) {
    throw parse_exception("error parsing XML: " + e.message());
  }

  test_results result;
  try {
    auto testsuites = tree.get_child("testsuites");
    for (auto& child : testsuites) {
      if (child.first == "testsuite") {
        auto& attributes = child.second.get_child("<xmlattr>");
        std::string name = attributes.get("name", "");
        unsigned tests = attributes.get<unsigned>("tests", 0),
                 failures = attributes.get<unsigned>("failures", 0),
                 disabled = attributes.get<unsigned>("disabled", 0),
                 errors = attributes.get<unsigned>("errors", 0),
                 time = attributes.get<unsigned>("time", 0);
        if (name.empty()) {
          throw parse_exception("error parsing XML: a <testsuite> has no name=");
        }
        result[name] = testsuite(std::move(name), tests, failures, disabled, errors, time);
      }
    }
  } catch (boost::property_tree::ptree_error e) {
    throw parse_exception(std::string("error decoding XML: ") + e.what());
  }

  if (result.empty()) {
    throw parse_exception("error parsing XML: does not contain any <testsuite> nodes");
  }

  return result;
}

class rubric_item {
private:
  std::string name_;
  unsigned points_;

public:

  rubric_item(std::string&& name, unsigned points)
  : name_(name), points_(points) { }

  rubric_item()
  : rubric_item(std::string(), 0) { }

  const std::string& name() const { return name_; }
  unsigned points() const { return points_; }
};

using rubric = std::vector<rubric_item>;

// throws parse_exception on I/O or parse error
rubric load_rubric(const std::string& json_path) {

  boost::property_tree::ptree tree;
  try {
    boost::property_tree::read_json(json_path, tree);
  } catch (boost::property_tree::json_parser_error e) {
    throw parse_exception("error parsing JSON: " + e.message());
  }

  rubric result;
  for (auto& child : tree) {
    auto& name = child.first;
    auto points = child.second.get_value<unsigned>(0);
    if (0 == points) {
      throw parse_exception("key '" + name + "' does not map to a positive integer");
    }
    result.emplace_back(std::string(name), points);
  }

  if (result.empty()) {
    throw parse_exception("JSON does not contain any rubric items");
  }

  return result;
}

class rubric_item_score {
private:
  const rubric_item* item_;
  bool is_correct_;

public:

  rubric_item_score(const rubric_item* item, bool is_correct)
  : item_(item), is_correct_(is_correct) {
    assert(nullptr != item);
    assert(is_valid());
  }

  rubric_item_score()
  : item_(nullptr), is_correct_(false) {
    assert(!is_valid());
  }

  bool is_valid() const { return bool(item_); }

  const rubric_item& item() const {
    assert(is_valid());
    return *item_;
  }

  bool is_correct() const {
    assert(is_valid());
    return is_correct_;
  }

  unsigned possible_points() const {
    assert(is_valid());
    return item_->points();
  }

  unsigned earned_points() const {
    assert(is_valid());
    if (is_correct_) {
      return possible_points();
    } else {
      return 0;
    }
  }
};

using rubric_score = std::vector<rubric_item_score>;

// throws parse_error if the testsuites is missing results for an item
// listed in the rubric
rubric_score evaluate_score(const rubric& the_rubric,
                            const test_results& the_results) {

  rubric_score result;
  // Note that we loop in the same order as the rubric as specified in the JSON
  // file. Presumably that is the order the end-user prefers. The results
  // are in a std::map that will always be in strict alphabetical order,
  // which is probably not the desired order.
  for (auto& item : the_rubric) {
    auto& name = item.name();
    auto suite_iter = the_results.find(name);
    if (suite_iter == the_results.end()) {
      throw parse_exception("testsuite '" + name +
                            "' from rubric cannot be found in googletest result XML");
    }
    auto& suite = suite_iter->second;
    assert(name == suite.name());
    bool worked = (0 == suite.failures());
    result.emplace_back(&item, worked);
  }
  return result;
}

void print_score(const rubric_score& the_score) {

  static const auto line = std::string(79, '=');

  std::cout << line << std::endl
            << "RUBRIC SCORE" << std::endl
            << line << std::endl;

  unsigned name_width = 0;
  for (auto& score : the_score) {
    name_width = std::max(name_width, unsigned(score.item().name().size()));
  }
  assert(name_width > 0);

  unsigned total_earned_points = 0, total_possible_points = 0;
  for (auto& score : the_score) {
    unsigned earned = score.earned_points(),
             possible = score.possible_points();
    total_earned_points += earned;
    total_possible_points += possible;
    std::cout << std::left
              << std::setw(name_width + 4) << score.item().name()
              << std::right
              << std::setw(4) << earned
              << " / "
              << std::setw(4) << possible
              << std::endl;
  }

  std::cout << line << std::endl
            << "TOTAL = "
            << std::setw(4) << total_earned_points
            << " / "
            << std::setw(4) << total_possible_points
            << std::endl;

  std::cout << line << std::endl << std::endl;
}

int main(int argc, char** argv) {

  // convert arguments to std::string
  std::vector<std::string> arguments;
  for (int i = 0; i < argc; ++i) {
    arguments.emplace_back(argv[i]);
  }

  if (arguments.size() != 3) {
    std::cout << "rubricscore usage:" << std::endl << std::endl
              << "    rubricscore <RUBRIC-JSON-PATH> <GTEST-XML-PATH>" << std::endl << std::endl;
    return 1;
  }

  auto& json_path = arguments[1],
        xml_path = arguments[2];

  rubric the_rubric;
  try {
    the_rubric = load_rubric(json_path);
  } catch (parse_exception e) {
    std::cerr << "rubricscore: error loading rubric JSON '" << json_path
              << "': " << e.what() << std::endl;
    return 1;
  }

  test_results the_results;
  try {
    the_results = load_test_results(xml_path);
  } catch (parse_exception e) {
    std::cerr << "rubricscore: error loading googletest XML '" << xml_path
              << "': " << e.what() << std::endl;
    return 1;
  }

  rubric_score the_score;
  try {
    the_score = evaluate_score(the_rubric, the_results);
  } catch (parse_exception e) {
    std::cerr << "rubricscore: " << e.what() << std::endl;
    return 1;
  }

  print_score(the_score);

  return 0;
}
