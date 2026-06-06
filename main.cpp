#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

/**
 * @brief Строит координатную матрицу 6x6 (полибианский квадрат) для шифра
 * Бифида.
 *
 * В нулевой строке и нулевом столбце матрицы располагаются цифры-метки
 * '1'..'5', а остальные ячейки (с 1-й по 5-ю строку и столбец) заполняются
 * буквами: сначала уникальными буквами ключа, затем недостающими буквами
 * алфавита. Буква 'j' исключается из алфавита.
 *
 * @param key Ключевое слово, задающее порядок заполнения матрицы буквами.
 * @return Матрица 6x6 (vector<vector<char>>): строка 0 и столбец 0 — цифры
 * '1'..'5', ячейки [1..5][1..5] — буквы алфавита без 'j'.
 */
vector<vector<char>> get_matrix_for_bifid_cipher(string key) {

  string key_n = "";

  for (char i : key) {

    if (i == 'j' || key_n.find(i) != string::npos) {
      continue;
    }

    key_n += i;
  }

  for (char i = 'a'; i <= 'z'; i++) {

    if (key_n.find(i) == string::npos && i != 'j') {
      key_n += i;
    }
  }

  vector<vector<char>> matrix(6);

  matrix[0] = {'1', '2', '3', '4', '5'};

  for (int i = 1; i < 6; i++) {
    matrix[i].push_back(matrix[0][i - 1]);
  }

  int k = 0;
  for (int h = 1; h < 6; h++) {
    for (int w = 1; w < 6; w++) {

      matrix[h].push_back(key_n[k]);
      k++;
    }
  }

  return matrix;
}

/**
 * @brief Шаг 1 шифра Бифида: замена каждого символа текста парой его координат
 * в матрице.
 *
 * Для каждого символа находится его позиция в матрице (полученной из ключа), и
 * символ заменяется парой меток: метка строки и метка столбца. Перед обработкой
 * буква 'j' заменяется на 'i'.
 *
 * @param text Исходный текст, состоящий из букв.
 * @param key Ключевое слово для построения матрицы Бифида.
 * @return Вектор пар координат: для каждого символа — вектор из двух символов
 *         {метка_строки, метка_столбца}.
 */
vector<vector<char>> bifid_cipher_step1(string text, string key) {

  vector<vector<char>> final_text;
  vector<vector<char>> matrix = get_matrix_for_bifid_cipher(key);

  replace(text.begin(), text.end(), 'j', 'i');

  for (char i : text) {

    int fh = -1;
    int fw = -1;
    bool found = false;

    for (int h = 1; h < 6; h++) {
      for (int w = 1; w < 6; w++) {

        if (i == matrix[h][w]) {
          fh = h;
          fw = w;
          found = true;
          break;
        }
      }
      if (found)
        break;
    }

    final_text.push_back({matrix[fh][0], matrix[0][fw - 1]});
  }

  return final_text;
}

/**
 * @brief Шаг 2 шифрования Бифида: перестановка координат и разбиение на пары.
 *
 * Координаты, полученные на шаге 1, переписываются по строкам: сначала подряд
 * все метки строк, затем все метки столбцов. Получившаяся последовательность
 * цифр разбивается на пары.
 *
 * @param text Исходный текст для шифрования.
 * @param key Ключевое слово для построения матрицы Бифида.
 * @return Вектор строк, каждая из которых - это пара цифр-координат.
 */
vector<string> bifid_cipher_step2(string text, string key) {

  vector<vector<char>> n_text = bifid_cipher_step1(text, key);
  string final_text = "";

  for (vector<char> i : n_text) {
    final_text += i[0];
  }

  for (vector<char> i : n_text) {
    final_text += i[1];
  }

  vector<string> pairs;

  for (int i = 0; i < final_text.length(); i += 2) {
    pairs.push_back(final_text.substr(i, 2));
  }

  return pairs;
}

/**
 * @brief Шаг 3 шифрования Бифида: преобразование пар координат обратно в буквы.
 *
 * Каждая пара цифр интерпретируется как координаты (строка, столбец) в матрице,
 * и из неё извлекается соответствующая буква - это и есть итоговый шифртекст.
 *
 * @param text Исходный текст для шифрования.
 * @param key Ключевое слово для построения матрицы Бифида.
 * @return Зашифрованный текст.
 */
string bifid_cipher_step3(string text, string key) {

  vector<string> pairs = bifid_cipher_step2(text, key);
  vector<vector<char>> matrix = get_matrix_for_bifid_cipher(key);

  string final_text = "";

  for (string i : pairs) {
    int h = i[0] - '0';
    int w = i[1] - '0';

    final_text += matrix[h][w];
  }

  return final_text;
}

/**
 * @brief Шаг 2 расшифрования Бифида: восстановление исходных пар координат.
 *
 * Координаты шифртекста (полученные шагом 1) выписываются в один ряд, затем ряд
 * делится пополам: первая половина - метки строк, вторая - метки столбцов.
 * Соответствующие элементы половин объединяются обратно в пары координат
 * каждого символа.
 *
 * @param text Шифрованный текст для расшифрования.
 * @param key Ключевое слово для построения матрицы Бифида.
 * @return Вектор строк, каждая из которых - пара цифр-координат одного
 * исходного символа.
 */
vector<string> bifid_cipher_decoding_step2(string text, string key) {

  vector<vector<char>> pairs = bifid_cipher_step1(text, key);

  string n_text = "";

  for (vector<char> i : pairs) {
    n_text += i[0];
    n_text += i[1];
  }

  vector<string> final_text;

  int half = n_text.length() / 2;

  for (int i = 0; i < half; i++) {

    string temp = "";
    temp += n_text[i];
    temp += n_text[i + half];
    final_text.push_back(temp);
  }

  return final_text;
}

/**
 * @brief Шаг 3 расшифрования Бифида: преобразование пар координат в буквы.
 *
 * Каждая восстановленная пара цифр интерпретируется как координаты (строка,
 * столбец) в матрице Бифида, и из неё извлекается исходная буква открытого
 * текста.
 *
 * @param text Шифрованный текст для расшифрования.
 * @param key Ключевое слово для построения матрицы Бифида.
 * @return Расшифрованный текст.
 */
string bifid_cipher_decoding_step3(string text, string key) {

  vector<string> pairs = bifid_cipher_decoding_step2(text, key);
  vector<vector<char>> matrix = get_matrix_for_bifid_cipher(key);

  string final_text = "";

  for (string i : pairs) {
    int h = i[0] - '0';
    int w = i[1] - '0';

    final_text += matrix[h][w];
  }

  return final_text;
}

// ------------------------------------- №7 -----------------------------------

string alphabet = "abcdefghijklmnopqrstuvwxyz";
int power = alphabet.length();

map<char, double> ENG_FREQS = {
    {'a', 8.17}, {'b', 1.49}, {'c', 2.78}, {'d', 4.25}, {'e', 12.70},
    {'f', 2.23}, {'g', 2.02}, {'h', 6.09}, {'i', 6.97}, {'j', 0.15},
    {'k', 0.77}, {'l', 4.03}, {'m', 2.41}, {'n', 6.75}, {'o', 7.51},
    {'p', 1.93}, {'q', 0.10}, {'r', 5.99}, {'s', 6.33}, {'t', 9.06},
    {'u', 2.76}, {'v', 0.98}, {'w', 2.36}, {'x', 0.15}, {'y', 1.97},
    {'z', 0.07}};

/**
 * @brief Находит повторяющиеся подстроки в тексте и расстояния между их
 * вхождениями.
 *
 * Для каждой подстроки фиксированной длины ищутся её повторные вхождения, и для
 * каждой найденной пары вхождений сохраняется расстояние.
 *
 * @param text Анализируемый шифртекст.
 * @param min_len Длина искомых подстрок.
 * @return Отображение {подстрока: список расстояний между её вхождениями}.
 */
map<string, vector<int>> find_repeats(const string &text, int min_len = 3) {

  map<string, vector<int>> rep;

  for (int i = 0; i + min_len <= text.length(); i++) {

    string s = text.substr(i, min_len);

    for (int j = i + 1; j + min_len <= text.length(); j++) {
      if (text.substr(j, min_len) == s) {
        rep[s].push_back(j - i);
      }
    }
  }

  return rep;
}

/**
 * @brief Собирает все расстояния между повторами в единый список.
 *
 * Объединяет значения из всех записей отображения, полученного функцией @ref
 * find_repeats.
 *
 * @param repeats Отображение {подстрока: список расстояний между её
 * вхождениями}.
 * @return Плоский вектор всех расстояний между повторяющимися подстроками.
 */
vector<int> get_distances(const map<string, vector<int>> &repeats) {

  vector<int> distances;

  for (auto const &p : repeats) {
    distances.insert(distances.end(), p.second.begin(), p.second.end());
  }

  return distances;
}

/**
 * @brief Оценивает вероятные длины ключа по расстояниям между повторами.
 *
 * Для каждого расстояния подсчитывается, на какие длины оно делится без
 * остатка; чем чаще длина является делителем, тем она вероятнее. Результат
 * сортируется по убыванию частоты.
 *
 * @param distances Список расстояний между повторами из @ref get_distances.
 * @param max_len Верхняя граница проверяемых длин ключа.
 * @return Вектор пар {длина_ключа, число_подходящих_расстояний},
 * отсортированный по убыванию частоты.
 */
vector<pair<int, int>> guess_key_lengths(const vector<int> &distances,
                                         int max_len = 20) {

  map<int, int> counts;

  for (int d : distances) {
    for (int i = 2; i < max_len; i++) {

      if (d % i == 0) {
        counts[i]++;
      }
    }
  }

  vector<pair<int, int>> result;

  for (auto const &p : counts) {
    result.push_back({p.first, p.second});
  }

  sort(result.begin(), result.end(),
       [](auto a, auto b) { return a.second > b.second; });

  return result;
}

/**
 * @brief Разбивает текст на группы по позиции символа относительно длины ключа.
 *
 * Символ с индексом i попадает в группу i % key_len. В итоге каждая группа
 * содержит символы, зашифрованные одним и тем же символом ключа.
 *
 * @param text Анализируемый шифртекст.
 * @param key_len Предполагаемая длина ключа (число групп).
 * @return Вектор из key_len строк-групп.
 */
vector<string> split_text(const string &text, int key_len) {
  vector<string> groups(key_len, "");

  for (int i = 0; i < text.length(); i++) {
    groups[i % key_len] += text[i];
  }

  return groups;
}

// Начало заимствования

/**
 * @brief Оценивает похожесть текста на английский по частотам букв.
 *
 * Подсчитывает фактическую частоту каждой буквы в тексте и суммирует абсолютные
 * отклонения от эталонных частот @ref ENG_FREQS. Чем меньше итоговое значение,
 * тем ближе распределение букв к английскому языку.
 *
 * @param text Текст для оценки.
 * @return Суммарное отклонение частот.
 */
double score_text_6(const string &text) {

  double score = 0;

  int N = text.length();

  map<char, int> freq;

  for (char c : text) {
    freq[c]++;
  }

  for (char c : alphabet) {
    double actual = 0;

    if (N > 0) {
      actual = (double)freq[c] / N * 100.0;
    }

    score += abs(actual - ENG_FREQS[c]);
  }

  return score;
}

// Конец заимствования

/**
 * @brief Подбирает наилучший сдвиг для одной группы символов.
 *
 * Перебирает все 26 возможных сдвигов, расшифровывает группу по формуле шифра
 * Бофора и выбирает сдвиг, дающий наименьшую оценку @ref score_text_6.
 *
 * @param group Группа символов, зашифрованных одним символом ключа.
 * @return Индекс наилучшего сдвига (0..25).
 */
int find_shift(const string &group) {

  int best_shift = 0;
  double best_score = 1e100;

  map<char, int> x;
  map<int, char> x_inv;

  for (int i = 0; i < power; i++) {
    x[alphabet[i]] = i;
    x_inv[i] = alphabet[i];
  }

  for (int shift = 0; shift < 26; shift++) {

    string decrypted = "";

    for (char c : group) {
      decrypted += x_inv[(shift - x[c] + 26) % 26];
    }

    double score = score_text_6(decrypted);

    if (score < best_score) {
      best_score = score;
      best_shift = shift;
    }
  }

  return best_shift;
}

/**
 * @brief Восстанавливает ключ по группам символов шифртекста.
 *
 * Для каждой группы определяется наилучший сдвиг через @ref find_shift, который
 * переводится в соответствующую букву ключа.
 *
 * @param groups Группы символов, полученные функцией @ref split_text.
 * @return Восстановленная строка-ключ.
 */
string find_key(const vector<string> &groups) {
  string key = "";
  map<int, char> x_inv;

  for (int i = 0; i < power; i++) {
    x_inv[i] = alphabet[i];
  }

  for (const string &g : groups) {
    int shift = find_shift(g);
    key += x_inv[shift];
  }

  return key;
}

/**
 * @brief Выполняет атаку Касиски на шифр Бофора и выводит найденный ключ.
 *
 * Последовательно находит повторы @ref find_repeats, извлекает расстояния
 * @ref get_distances, оценивает вероятную длину ключа @ref guess_key_lengths,
 * разбивает текст на группы @ref split_text и восстанавливает ключ @ref
 * find_key. Результат печатается в стандартный поток вывода.
 *
 * @param cipher Анализируемый шифртекст.
 * @return Функция ничего не возвращает; результат выводится на экран.
 */
void kasiski_attack(const string &cipher) {

  auto repeats = find_repeats(cipher);

  auto distances = get_distances(repeats);

  if (distances.empty()) {
    cout << "Повторы не найдены" << endl;
    return;
  }

  auto candidates = guess_key_lengths(distances);

  int key_len = candidates[0].first;

  auto groups = split_text(cipher, key_len);

  string key = find_key(groups);

  cout << "Найденный ключ: " << key << endl;
}

// ------------------------------------- №6 -----------------------------------

map<string, double> ENGLISH_TRIGRAMS = {
    {"the", 10.0}, {"and", 9.0}, {"ing", 8.0}, {"ent", 7.0}, {"ion", 6.5},
    {"her", 6.0},  {"for", 5.5}, {"tha", 5.0}, {"nth", 4.5}, {"int", 4.0},
    {"ere", 3.5},  {"tth", 3.0}, {"pro", 2.5}, {"con", 2.0}, {"ter", 1.5},
    {"ati", 1.0},  {"hat", 1.0}, {"all", 1.0}, {"eth", 1.0}, {"hes", 1.0}};

/**
 * @brief Оценивает осмысленность текста по сумме весов встречающихся триграмм.
 *
 * Для каждой триграммы текста, присутствующей в таблице ENGLISH_TRIGRAMS,
 * к итоговой оценке добавляется её вес. Чем выше значение, тем больше он
 * соответствует правильному английскому тексту.
 *
 * @param text Текст для оценки.
 * @return Суммарный вес найденных триграмм.
 */
double score_text(const string &text) {

  double score = 0;

  for (int i = 0; i + 2 < text.length(); i++) {

    string trigram = text.substr(i, 3);

    if (ENGLISH_TRIGRAMS.find(trigram) != ENGLISH_TRIGRAMS.end()) {
      score += ENGLISH_TRIGRAMS[trigram];
    }
  }

  return score;
}

/**
 * @brief Расшифровывает текст шифра перестановки по столбцам при заданной
 * перестановке.
 *
 * Восстанавливает длины столбцов, распределяет
 * символы шифртекста по столбцам согласно перестановке и считывает их
 * построчно, получая открытый текст.
 *
 * @param text Шифртекст для расшифрования.
 * @param perm Перестановка столбцов (порядок, в котором столбцы были выписаны
 * при шифровании).
 * @return Расшифрованный текст.
 */
string decrypt_with_permutation(const string &text, const vector<int> &perm) {

  int n = perm.size();
  int rows = text.length() / n;
  int extra = text.length() % n;

  vector<int> col_lengths(n, rows);

  for (int i = 0; i < extra; i++) {
    col_lengths[i]++;
  }

  vector<string> columns(n, "");
  int pos = 0;

  for (int original_index : perm) {

    int length = col_lengths[original_index];

    columns[original_index] = text.substr(pos, length);

    pos += length;
  }

  int max_rows = *max_element(col_lengths.begin(), col_lengths.end());

  string p_text = "";
  for (int row = 0; row < max_rows; row++) {
    for (int col = 0; col < n; col++) {
      if (row < columns[col].length()) {
        p_text += columns[col][row];
      }
    }
  }

  return p_text;
}

/**
 * @brief Результат полного перебора перестановок для одной длины ключа.
 */
struct BruteForceResult {
  string text;
  vector<int> perm;
  double score;
};

/**
 * @brief Полный перебор всех перестановок столбцов для фиксированной длины
 * ключа.
 *
 * Перебирает все перестановки длины key_length, для каждой расшифровывает текст
 * @ref decrypt_with_permutation и оценивает его @ref score_text, запоминая
 * лучший вариант.
 *
 * @param ciphertext Шифртекст для взлома.
 * @param key_length Предполагаемая длина ключа (число столбцов).
 * @return Структура @ref BruteForceResult с лучшим текстом, его перестановкой и
 * оценкой.
 */
BruteForceResult brute_force(const string &ciphertext, int key_length) {

  double best_score = -1;
  string best_text = "";
  vector<int> best_perm;

  vector<int> perm(key_length);

  for (int i = 0; i < key_length; i++) {
    perm[i] = i;
  }

  do {
    string current_text = decrypt_with_permutation(ciphertext, perm);

    double current_score = score_text(current_text);

    if (current_score > best_score) {
      best_score = current_score;
      best_text = current_text;
      best_perm = perm;
    }

  } while (next_permutation(perm.begin(), perm.end()));

  return {best_text, best_perm, best_score};
}

/**
 * @brief Взламывает шифр перестановки по столбцам при неизвестной длине ключа.
 *
 * Перебирает длины ключа от 2 до max_length, для каждой выполняет полный
 * перебор
 * @ref brute_force и выбирает вариант с наибольшей оценкой. Найденная длина
 * ключа, порядок столбцов и расшифрованный текст выводятся на экран.
 *
 * @param ciphertext Шифртекст для взлома.
 * @param max_length Максимальная длина ключа.
 * @return Всегда возвращает 0; результат выводится на экран.
 */
int crack_with_unknown_key_length(const string &ciphertext,
                                  int max_length = 8) {

  double best_score = -1;
  string best_text = "";
  int best_length = 0;
  vector<int> best_perm;

  for (int key_length = 2; key_length <= max_length; key_length++) {

    BruteForceResult result = brute_force(ciphertext, key_length);

    if (result.score > best_score) {

      best_score = result.score;
      best_text = result.text;
      best_length = key_length;
      best_perm = result.perm;
    }
  }

  cout << "Наиболее вероятная длина ключа: " << best_length << endl;

  cout << "Порядок столбцов: ";

  for (int x : best_perm) {
    cout << x << " ";
  }

  cout << endl;

  cout << "Расшифрованный текст:\n" << best_text << endl;

  return 0;
}

// ------------------------------------- №4 -----------------------------------

/**
 * @brief Строит матрицу 6x6 для шифра ADFGX, запрашивая алфавит у пользователя.
 *
 * В нулевой строке и нулевом столбце располагаются буквы 'a','d','f','g','x'.
 * Ячейки [1...5][1...5] заполняются алфавитом: при вводе "1" используется
 * стандартный латинский алфавит без 'j', иначе — введённая пользователем
 * строка. Ввод читается со стандартного потока ввода внутри функции.
 *
 * @return Матрица 6x6 (vector<vector<char>>): строка 0 и столбец 0 — метки
 * ADFGX, ячейки [1...5][1...5] — буквы алфавита.
 */
vector<vector<char>> get_matrix_for_adfgx_cipher() {

  vector<vector<char>> matrix(6);

  matrix[0] = {'a', 'd', 'f', 'g', 'x'};

  for (int i = 1; i < 6; i++) {
    matrix[i].push_back(matrix[0][i - 1]);
  }

  string alphabet1 = "";
  cout << "Введите атфавит для составление матрицы (нажмите 1 для выбора "
          "стандартного алфавита)"
       << endl;
  string p = "";
  cin >> p;
  cin.ignore(10000, '\n');

  if (p == "1") {
    for (char i = 'a'; i <= 'z'; i++) {
      if (i != 'j') {
        alphabet1 += i;
      }
    }
  } else {
    for (char i : p) {
      alphabet1 += i;
    }
  }

  int k = 0;
  for (int h = 1; h < 6; h++) {
    for (int w = 1; w < 6; w++) {

      matrix[h].push_back(alphabet1[k]);
      k++;
    }
  }

  return matrix;
}

/**
 * @brief Шаг 1 шифра ADFGX: замена каждой буквы парой меток (строка, столбец).
 *
 * Для каждого символа находится его позиция в матрице ADFGX, и он заменяется
 * парой буквенных меток (метка строки и метка столбца). Перед обработкой 'j'
 * заменяется на 'i'.
 *
 * @param text Исходный текст для преобразования.
 * @return Строка из меток ADFGX.
 */
string adfgx_cipher_step1(string text) {

  string final_text = "";
  auto matrix = get_matrix_for_adfgx_cipher();

  replace(text.begin(), text.end(), 'j', 'i');

  for (char i : text) {

    int fh = -1;
    int fw = -1;
    bool found = false;

    for (int h = 1; h < 6; h++) {
      for (int w = 1; w < 6; w++) {

        if (i == matrix[h][w]) {
          fh = h;
          fw = w;
          found = true;
          break;
        }
      }
      if (found)
        break;
    }

    final_text += matrix[fh][0];
    final_text += matrix[0][fw - 1];
  }

  return final_text;
}

/**
 * @brief Шаг 2 шифрования ADFGX: столбцовая перестановка меток по ключу.
 *
 * Строка меток из @ref adfgx_cipher_step1 разбивается на строки длиной с ключ,
 * столбцы сортируются по алфавитному порядку символов ключа и считываются по
 * столбцам, образуя итоговый шифртекст.
 *
 * @param text Исходный текст для шифрования.
 * @param key Ключевое слово, задающее порядок перестановки столбцов.
 * @return Зашифрованный текст в метках ADFGX.
 */
string adfgx_cipher_step2(string text, string key) {

  string text_n = adfgx_cipher_step1(text);
  int n = key.length();

  vector<string> rows;
  for (int i = 0; i < text_n.length(); i += n) {
    rows.push_back(text_n.substr(i, n));
  }

  vector<pair<int, char>> indexed_key;
  for (int i = 0; i < n; i++) {
    indexed_key.push_back({i, key[i]});
  }

  vector<pair<int, char>> sorted_key = indexed_key;
  sort(sorted_key.begin(), sorted_key.end(),
       [](const pair<int, char> &a, const pair<int, char> &b) {
         if (a.second == b.second)
           return a.first < b.first;
         return a.second < b.second;
       });

  string cipher_text = "";
  for (const auto &item : sorted_key) {
    int original_index = item.first;

    for (const string &row : rows) {
      if (original_index < row.length()) {

        cipher_text += row[original_index];
      }
    }
  }

  return cipher_text;
}

/**
 * @brief Шаг 1 расшифрования ADFGX: отмена столбцовой перестановки меток.
 *
 * По ключу восстанавливаются длины столбцов, шифртекст распределяется обратно
 * по столбцам в порядке сортировки ключа и считывается построчно,
 * восстанавливая исходную последовательность меток ADFGX.
 *
 * @param text Шифртекст в метках ADFGX.
 * @param key Ключевое слово, использованное при шифровании.
 * @return Строка меток ADFGX в порядке до перестановки.
 */
string adfgx_cipher_decoding_step1(string text, string key) {

  int n = key.length();

  vector<pair<int, char>> indexed_key;
  for (int i = 0; i < n; i++) {
    indexed_key.push_back({i, key[i]});
  }

  vector<pair<int, char>> sorted_key = indexed_key;
  sort(sorted_key.begin(), sorted_key.end(),
       [](const pair<int, char> &a, const pair<int, char> &b) {
         if (a.second == b.second)
           return a.first < b.first;
         return a.second < b.second;
       });

  int rows_count = text.length() / n;
  int extra = text.length() % n;

  vector<int> col_lengths(n, rows_count);
  for (int i = 0; i < extra; i++) {
    col_lengths[i] += 1;
  }

  vector<string> columns(n, "");
  int pos = 0;

  for (const auto &item : sorted_key) {
    int original_index = item.first;
    int length = col_lengths[original_index];
    columns[original_index] = text.substr(pos, length);
    pos += length;
  }

  string p_text = "";
  int max_rows = 0;
  for (int len : col_lengths) {
    if (len > max_rows)
      max_rows = len;
  }

  for (int row = 0; row < max_rows; row++) {
    for (int col = 0; col < n; col++) {
      if (row < columns[col].length()) {
        p_text += columns[col][row];
      }
    }
  }

  return p_text;
}

/**
 * @brief Шаг 2 расшифрования ADFGX: разбиение восстановленных меток на пары.
 *
 * Последовательность меток из @ref adfgx_cipher_decoding_step1 делится на пары,
 * каждая из которых задаёт координаты одной буквы в матрице ADFGX.
 *
 * @param text Шифртекст в метках ADFGX.
 * @param key Ключевое слово, использованное при шифровании.
 * @return Вектор строк-пар меток.
 */
vector<string> adfgx_cipher_decoding_step2(string text, string key) {

  string text_n = adfgx_cipher_decoding_step1(text, key);
  vector<string> pairs;

  for (int i = 0; i < text_n.length(); i += 2) {
    pairs.push_back(text_n.substr(i, 2));
  }

  return pairs;
}

/**
 * @brief Шаг 3 расшифрования ADFGX: преобразование пар меток в буквы.
 *
 * Для каждой пары меток первая определяет строку, вторая - столбец матрицы
 * ADFGX, и из соответствующей ячейки извлекается исходная буква.
 *
 * @param text Шифртекст в метках ADFGX.
 * @param key Ключевое слово, использованное при шифровании.
 * @return Расшифрованный текст.
 */
string adfgx_cipher_decoding_step3(string text, string key) {

  vector<string> pairs = adfgx_cipher_decoding_step2(text, key);
  string final_text = "";
  auto matrix = get_matrix_for_adfgx_cipher();

  for (const string &p : pairs) {

    char first = p[0];
    char second = p[1];

    int f = -1;
    int s = -1;

    for (int h = 1; h < 6; h++) {
      if (matrix[h][0] == first) {
        f = h;
        break;
      }
    }

    for (int w = 0; w < 5; w++) {
      if (matrix[0][w] == second) {
        s = w + 1;
        break;
      }
    }

    final_text += matrix[f][s];
  }

  return final_text;
}

// ------------------------------------- №3 -----------------------------------

/**
 * @brief Подготавливает текст к шифрованию Плейфером: разбивка на биграммы.
 *
 * Заменяет 'j' на 'i', затем разбивает текст на пары букв. Если в паре две
 * одинаковые буквы или текст имеет нечётную длину, в пару добавляется
 * буква-заполнитель 'x'.
 *
 * @param text Исходный текст для подготовки.
 * @return Вектор биграмм (строк по 2 символа), готовых к шифрованию.
 */
vector<string> preparing_text_for_the_playfair_cipher(string text) {

  for (char &c : text) {
    if (c == 'j') {
      c = 'i';
    }
  }

  vector<string> prepare_text;

  int i = 0;
  while (i < text.length()) {

    if (i + 1 >= text.length()) {

      string temp = "";
      temp += text[i];
      temp += 'x';

      prepare_text.push_back(temp);
      break;
    }

    if (text[i] == text[i + 1]) {

      string temp = "";
      temp += text[i];
      temp += 'x';

      prepare_text.push_back(temp);

      i += 1;
    } else {

      string temp = "";
      temp += text[i];
      temp += text[i + 1];

      prepare_text.push_back(temp);

      i += 2;
    }
  }

  return prepare_text;
}

/**
 * @brief Строит ключевую матрицу 5x5 для шифра Плейфера.
 *
 * Сначала в матрицу заносятся уникальные буквы ключа без 'j' и повторяющихся
 * букв, затем недостающие буквы алфавита, также без 'j', последовательно
 * заполняя строки.
 *
 * @param key Ключевое слово.
 * @return Матрица 5x5 (vector<vector<char>>) букв алфавита без 'j' и повторов.
 */
vector<vector<char>> make_key_matrix(string key) {

  string key_n = "";

  for (char i : key) {

    if (i == 'j' || key_n.find(i) != string::npos) {
      continue;
    }

    key_n += i;
  }

  for (char i = 'a'; i <= 'z'; i++) {

    if (key_n.find(i) == string::npos && i != 'j') {
      key_n += i;
    }
  }

  vector<vector<char>> key_m(5, vector<char>(5));

  int i = 0;
  for (int h = 0; h < 5; h++) {
    for (int w = 0; w < 5; w++) {

      key_m[h][w] = key_n[i];

      i++;
    }
  }

  return key_m;
}

/**
 * @brief Шифрует или расшифровывает текст шифром Плейфера.
 *
 * Строит ключевую матрицу @ref make_key_matrix, разбивает текст на биграммы
 * @ref preparing_text_for_the_playfair_cipher и для каждой пары применяет
 * правила Плейфера (одна строка / один столбец / прямоугольник) в зависимости
 * от режима.
 *
 * @param text Исходный текст для обработки.
 * @param key Ключевое слово для построения матрицы.
 * @param mode Режим работы: "encrypt" — шифрование, "decrypt" — расшифрование.
 * @return Результат шифрования или расшифрования.
 */
string playfair_cipher(string text, string key, string mode = "encrypt") {

  vector<vector<char>> key_m = make_key_matrix(key);
  vector<string> prepare_text = preparing_text_for_the_playfair_cipher(text);

  string final_text = "";

  for (string i : prepare_text) {

    char first = i[0];
    char second = i[1];

    vector<int> f;
    vector<int> s;

    for (int h = 0; h < 5; h++) {
      for (int w = 0; w < 5; w++) {

        if (key_m[h][w] == first) {
          f = {h, w};
        }

        if (key_m[h][w] == second) {
          s = {h, w};
        }
      }
    }

    if (mode == "encrypt") {

      if (f[0] == s[0]) {

        final_text += key_m[f[0]][(f[1] + 1) % 5];
        final_text += key_m[s[0]][(s[1] + 1) % 5];

      } else if (f[1] == s[1]) {

        final_text += key_m[(f[0] + 1) % 5][f[1]];
        final_text += key_m[(s[0] + 1) % 5][s[1]];

      } else {

        final_text += key_m[f[0]][s[1]];
        final_text += key_m[s[0]][f[1]];
      }
    } else if (mode == "decrypt") {

      if (f[0] == s[0]) {

        final_text += key_m[f[0]][(f[1] - 1 + 5) % 5];
        final_text += key_m[s[0]][(s[1] - 1 + 5) % 5];

      } else if (f[1] == s[1]) {

        final_text += key_m[(f[0] - 1 + 5) % 5][f[1]];
        final_text += key_m[(s[0] - 1 + 5) % 5][s[1]];

      } else {

        final_text += key_m[f[0]][s[1]];
        final_text += key_m[s[0]][f[1]];
      }
    }
  }

  return final_text;
}

// ------------------------ №2 ------------------------------------------

/**
 * @brief Шифрует или расшифровывает текст шифром Бофора.
 *
 * Применяет формулу Бофора посимвольно: результат = (гамма - текст) mod power.
 * Шифрование и расшифрование выполняются одинаково.
 *
 * @param text Исходный текст в виде вектора символов.
 * @param gama Гамма, не короче текста.
 * @param mode Режим работы: "encrypt" или "decrypt".
 * @return Результат преобразования (строка).
 */
string beauford_cipher(const vector<char> &text, const string &gama,
                       const string &mode = "encrypt") {

  map<char, int> x;
  map<int, char> x_inv;

  for (int i = 0; i < power; i++) {
    x['a' + i] = i;
    x_inv[i] = 'a' + i;
  }

  string y = "";

  if (mode == "encrypt") {
    for (int i = 0; i < text.size(); i++) {
      y += x_inv[(x[gama[i]] - x[text[i]] + power) % power];
    }

    return y;
  }

  string t = "";

  for (int i = 0; i < text.size(); i++) {
    t += x_inv[(x[gama[i]] - x[text[i]] + power) % power];
  }

  return t;
}

/**
 * @brief Формирует гамму нужной длины повторением ключа.
 *
 * Многократно дописывает ключ, пока длина строки не достигнет требуемой.
 *
 * @param key Ключевое слово.
 * @param len_text Минимально необходимая длина гаммы.
 * @return Гамма, образованная повторением ключа, длиной не меньше len_text.
 */
string make_gamma1(const string &key, int len_text) {

  string g = "";

  while (g.length() < len_text) {
    g += key;
  }

  return g;
}

// --------------- Шифр №1 ------------------------------

/**
 * @brief Шифрует текст шифром перестановки по столбцам.
 *
 * Текст записывается в таблицу построчно (ширина равна длине ключа), столбцы
 * упорядочиваются по алфавитному порядку символов ключа и считываются по
 * столбцам, образуя шифртекст.
 *
 * @param text Исходный текст для шифрования.
 * @param key Ключевое слово, задающее порядок столбцов.
 * @return Зашифрованный текст.
 */
string Columnar_Transposition_Cipher_e(string &text, const string &key) {

  int n = key.length();

  vector<string> rows;
  for (int i = 0; i < text.length(); i += n) {
    rows.push_back(text.substr(i, n));
  }

  vector<pair<int, char>> indexed_key;
  for (int i = 0; i < n; i++) {
    indexed_key.push_back({i, key[i]});
  }

  vector<pair<int, char>> sorted_key = indexed_key;
  sort(sorted_key.begin(), sorted_key.end(),
       [](const pair<int, char> &a, const pair<int, char> &b) {
         if (a.second == b.second)
           return a.first < b.first;
         return a.second < b.second;
       });

  string cipher_text = "";
  for (const auto &item : sorted_key) {
    int original_index = item.first;

    for (const string &row : rows) {
      if (original_index < row.length()) {

        cipher_text += row[original_index];
      }
    }
  }

  return cipher_text;
}

/**
 * @brief Расшифровывает текст шифра перестановки по столбцам.
 *
 * По ключу восстанавливаются длины столбцов,
 * шифртекст распределяется по столбцам в порядке сортировки ключа и считывается
 * построчно, восстанавливая открытый текст.
 *
 * @param text Шифртекст для расшифрования.
 * @param key Ключевое слово, использованное при шифровании.
 * @return Расшифрованный текст.
 */
string Columnar_Transposition_Cipher_d(const string &text, const string &key) {

  int n = key.length();

  vector<pair<int, char>> indexed_key;
  for (int i = 0; i < n; i++) {
    indexed_key.push_back({i, key[i]});
  }

  vector<pair<int, char>> sorted_key = indexed_key;
  sort(sorted_key.begin(), sorted_key.end(),
       [](const pair<int, char> &a, const pair<int, char> &b) {
         if (a.second == b.second)
           return a.first < b.first;
         return a.second < b.second;
       });

  int rows_count = text.length() / n;
  int extra = text.length() % n;

  vector<int> col_lengths(n, rows_count);
  for (int i = 0; i < extra; i++) {
    col_lengths[i] += 1;
  }

  vector<string> columns(n, "");
  int pos = 0;

  for (const auto &item : sorted_key) {
    int original_index = item.first;
    int length = col_lengths[original_index];
    columns[original_index] = text.substr(pos, length);
    pos += length;
  }

  string p_text = "";
  int max_rows = 0;
  for (int len : col_lengths) {
    if (len > max_rows)
      max_rows = len;
  }

  for (int row = 0; row < max_rows; row++) {
    for (int col = 0; col < n; col++) {
      if (row < columns[col].length()) {
        p_text += columns[col][row];
      }
    }
  }

  return p_text;
}

/**
 * @brief Точка входа программы: интерактивное меню выбора шифра.
 *
 * Настраивает кодировку консоли на UTF-8 и в цикле предлагает пользователю
 * выбрать операцию: шифрование/расшифрование одним из шифров (перестановка по
 * столбцам, Бофор, Плейфер, ADFGX, Бифид) с чтением исходного текста из файла и
 * записью результата в файл, либо криптоанализ (перестановки по столбцам или
 * Бофора). Цикл завершается при вводе "0".
 *
 * @return 0.
 */
#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main() {

  SetConsoleCP(65001);
  SetConsoleOutputCP(65001);

  while (true) {
    cout << "Выберите шифр" << endl;
    cout << "1. Шифр перестановки по столбцам" << endl;
    cout << "2. Шифр Бофорда" << endl;
    cout << "3. Шифр Плейфера" << endl;
    cout << "4. Шифр ADFGX" << endl;
    cout << "5. Шифр Бифида" << endl;
    cout << "6. Криптоанализ шифра перестановки по столбцам" << endl;
    cout << "7. Криптоанализ шифра Бофорда" << endl;
    cout << "0. Выход" << endl;

    string m;
    cin >> m;
    try {
      if (m == "1") {
        cout << "1. Зашифровать 2. Расшифровать" << endl;

        string p;
        cin >> p;
        cin.ignore();

        cout << "Введите путь к исходному файлу (.txt): ";
        string in_path;
        getline(cin, in_path);
        ifstream infile(in_path);
        if (!infile) {
          throw runtime_error("Не удалось открыть файл: " + in_path);
        }

        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        cout << "Введите ключ: ";
        string key;
        getline(cin, key);
        if (key.empty()) {
          throw runtime_error("Ключ не может быть пустым");
        }

        for (char &c : text)
          c = tolower(c);
        for (char &c : key)
          c = tolower(c);

        string filtered_text;
        for (char c : text) {
          if (c >= 'a' && c <= 'z') {
            filtered_text += c;
          }
        }

        string res;
        if (p == "1") {
          res = Columnar_Transposition_Cipher_e(filtered_text, key);
        } else {
          res = Columnar_Transposition_Cipher_d(filtered_text, key);
        }

        cout << "Введите путь к файлу для сохранения результата (.txt): ";
        string out_path;
        getline(cin, out_path);

        ofstream outfile(out_path);
        if (!outfile) {
          throw runtime_error("Не удалось создать файл: " + out_path);
        }
        outfile << res;
        outfile.close();

      } else if (m == "2") {
        cout << "1. Зашифровать 2. Расшифровать" << endl;

        string p;
        cin >> p;
        cin.ignore();

        cout << "Введите путь к исходному файлу (.txt): ";
        string in_path;
        getline(cin, in_path);

        ifstream infile(in_path);
        if (!infile) {
          throw runtime_error("Не удалось открыть файл: " + in_path);
        }
        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        cout << "Введите ключ: " << endl;
        string key;
        getline(cin, key);
        if (key.empty()) {
          throw runtime_error("Ключ не может быть пустым");
        }

        for (char &c : text)
          c = tolower(c);
        for (char &c : key)
          c = tolower(c);

        vector<char> filtered_text;
        for (char c : text) {
          if (c >= 'a' && c <= 'z') {
            filtered_text.push_back(c);
          }
        }

        string gamma = make_gamma1(key, filtered_text.size());

        string mode;
        if (p == "1") {
          mode = "encrypt";
        } else {
          mode = "decrypt";
        }

        string res = beauford_cipher(filtered_text, gamma, mode);

        cout << "Введите путь к файлу для сохранения результата (.txt): ";
        string out_path;
        getline(cin, out_path);

        ofstream outfile(out_path);
        if (!outfile) {
          throw runtime_error("Не удалось создать файл: " + out_path);
        }
        outfile << res;
        outfile.close();

      } else if (m == "3") {

        cout << "1. Зашифровать 2. Расшифровать" << endl;

        string p;
        cin >> p;

        cin.ignore();

        cout << "Введите путь к исходному файлу (.txt): ";
        string in_path;
        getline(cin, in_path);

        ifstream infile(in_path);
        if (!infile) {
          throw runtime_error("Не удалось открыть файл: " + in_path);
        }
        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        cout << "Введите ключ: " << endl;
        string key;
        getline(cin, key);
        if (key.empty()) {
          throw runtime_error("Ключ не может быть пустым");
        }

        for (char &c : text)
          c = tolower(c);
        for (char &c : key)
          c = tolower(c);

        string filtered_text;
        for (char c : text) {
          if (c >= 'a' && c <= 'z') {
            filtered_text += c;
          }
        }

        string mode;
        if (p == "1") {
          mode = "encrypt";
        } else {
          mode = "decrypt";
        }

        string res = playfair_cipher(filtered_text, key, mode);

        cout << "Введите путь к файлу для сохранения результата (.txt): ";
        string out_path;
        getline(cin, out_path);

        ofstream outfile(out_path);
        if (!outfile) {
          throw runtime_error("Не удалось создать файл: " + out_path);
        }
        outfile << res;
        outfile.close();

      } else if (m == "4") {

        cout << "1. Зашифровать 2. Расшифровать" << endl;

        string p;
        cin >> p;
        cin.ignore();

        cout << "Введите путь к исходному файлу: ";
        string in_path;
        getline(cin, in_path);

        ifstream infile(in_path);
        if (!infile) {
          throw runtime_error("Не удалось открыть файл: " + in_path);
        }
        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        cout << "Введите ключ: " << endl;
        string key;
        getline(cin, key);
        if (key.empty()) {
          throw runtime_error("Ключ не может быть пустым");
        }

        for (char &c : text)
          c = tolower(c);
        for (char &c : key)
          c = tolower(c);

        string filtered_text;
        for (char c : text) {
          if (c >= 'a' && c <= 'z') {
            filtered_text += c;
          }
        }

        string res;
        if (p == "1") {
          res = adfgx_cipher_step2(filtered_text, key);
        } else {
          res = adfgx_cipher_decoding_step3(filtered_text, key);
        }

        cout << "Введите путь к файлу для сохранения результата: ";
        string out_path;
        getline(cin, out_path);

        ofstream outfile(out_path);
        if (!outfile) {
          throw runtime_error("Не удалось создать файл: " + out_path);
        }
        outfile << res;
        outfile.close();

      } else if (m == "6") {

        string cipher;
        cout << "Введите шифртекст: ";
        cin >> cipher;

        crack_with_unknown_key_length(cipher, 8);

      } else if (m == "7") {

        string cipher;
        cout << "Введите шифртекст: ";
        cin >> cipher;

        kasiski_attack(cipher);

      } else if (m == "5") {

        cout << "1. Зашифровать 2. Расшифровать" << endl;

        string p;
        cin >> p;

        cin.ignore();

        cout << "Введите путь к исходному файлу (.txt): ";
        string in_path;
        getline(cin, in_path);

        ifstream infile(in_path);
        if (!infile) {
          throw runtime_error("Не удалось открыть файл: " + in_path);
        }
        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();

        cout << "Введите ключ: " << endl;
        string key;
        getline(cin, key);
        if (key.empty()) {
          throw runtime_error("Ключ не может быть пустым");
        }

        for (char &c : text)
          c = tolower(c);
        for (char &c : key)
          c = tolower(c);

        string filtered_text;
        for (char c : text) {
          if (c >= 'a' && c <= 'z') {
            filtered_text += c;
          }
        }

        string res;
        if (p == "1") {
          res = bifid_cipher_step3(filtered_text, key);
        } else {
          res = bifid_cipher_decoding_step3(filtered_text, key);
        }

        cout << "Введите путь к файлу для сохранения результата (.txt): ";
        string out_path;
        getline(cin, out_path);

        ofstream outfile(out_path);
        if (!outfile) {
          throw runtime_error("Не удалось создать файл: " + out_path);
        }
        outfile << res;
        outfile.close();

      } else if (m == "0") {
        break;
      }
    } catch (const exception &e) {
      cout << "Ошибка: " << e.what() << endl;
    }
  }
}
#endif
