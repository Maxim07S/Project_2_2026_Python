#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "main.cpp"

TEST_CASE("make_gamma1 repeats key correctly") {
  CHECK(make_gamma1("abc", 7) == "abcabcabc");
  CHECK(make_gamma1("k", 5) == "kkkkk");
}

TEST_CASE("make_gamma1 edge cases") {
  CHECK(make_gamma1("abc", 0) == "");
  CHECK(make_gamma1("abc", 2) == "abc");
}

TEST_CASE("make_gamma1 negative cases") {
  CHECK(make_gamma1("abc", 6) != make_gamma1("xyz", 6));
  CHECK(make_gamma1("abc", 4) != "abc");
}

TEST_CASE("Beaufort cipher encrypt/decrypt") {
  vector<char> text = {'h', 'e', 'l', 'l', 'o'};

  string gamma = make_gamma1("key", text.size());

  string cipher = beauford_cipher(text, gamma);

  vector<char> cipher_vec(cipher.begin(), cipher.end());

  string decoded = beauford_cipher(cipher_vec, gamma);

  CHECK(decoded == "hello");

  CHECK(cipher != "hello");

  string wrong_gamma = make_gamma1("xyz", text.size());
  CHECK(beauford_cipher(cipher_vec, wrong_gamma) != "hello");
}

TEST_CASE("Preparing text for Playfair") {
  auto result = preparing_text_for_the_playfair_cipher("hello");

  REQUIRE(result.size() == 3);

  CHECK(result[0] == "he");
  CHECK(result[1] == "lx");
  CHECK(result[2] == "lo");
}

TEST_CASE("Preparing text for Playfair odd length") {
  auto result = preparing_text_for_the_playfair_cipher("cat");

  REQUIRE(result.size() == 2);

  CHECK(result[0] == "ca");
  CHECK(result[1] == "tx");
}

TEST_CASE("Preparing text for Playfair negative cases") {

  auto r1 = preparing_text_for_the_playfair_cipher("jump");
  for (auto bigram : r1)
    for (char c : bigram)
      CHECK(c != 'j');

  auto r2 = preparing_text_for_the_playfair_cipher("aa");
  CHECK(r2[0] != "aa");
}

TEST_CASE("Columnar transposition encrypt decrypt") {
  string text = "helloworld";
  string key = "zebra";

  string cipher = Columnar_Transposition_Cipher_e(text, key);
  string decoded = Columnar_Transposition_Cipher_d(cipher, key);

  CHECK(decoded == text);
}

TEST_CASE("Columnar transposition negative cases") {
  string text = "helloworld";
  string key = "zebra";

  string cipher = Columnar_Transposition_Cipher_e(text, key);

  CHECK(cipher != text);

  string wrong = Columnar_Transposition_Cipher_d(cipher, "abcde");
  CHECK(wrong != text);
}

TEST_CASE("make_key_matrix removes duplicates and j") {
  auto matrix = make_key_matrix("hello");

  CHECK(matrix[0][0] == 'h');
  CHECK(matrix[0][1] == 'e');
  CHECK(matrix[0][2] == 'l');
  CHECK(matrix[0][3] == 'o');

  bool has_j = false;

  for (auto row : matrix) {
    for (char c : row) {
      if (c == 'j') {
        has_j = true;
      }
    }
  }

  CHECK_FALSE(has_j);
}

TEST_CASE("make_key_matrix negative cases") {
  auto matrix = make_key_matrix("aab");

  int count_a = 0;
  for (auto row : matrix)
    for (char c : row)
      if (c == 'a')
        count_a++;
  CHECK(count_a == 1);

  int count_j = 0;
  for (auto &row : matrix)
    for (char c : row)
      if (c == 'j')
        count_j++;
  CHECK(count_j == 0);
}
