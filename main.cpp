#include <bits/stdc++.h>

using namespace std;

// Input file paths
// auto input_files = { "eg.in", "small.in", "medium.in", "big.in" };
// auto test_input_files = { "eg.in" };

#define MUSHROOM 'M'
#define TOMATO 'T'
#define EMPTY '_'

// Global variables
int R, C, L, H;

class Point {
public:
    int r;
    int c;

    Point (int r, int c): r(r), c(c) {}

    Point () {}

    void print_output () {
        cout << r << " " << c << " ";
    }
};

class Pizza;
class Slice {
public:
    Point beg;
    Point end;

    Slice (Point & beg, Point & end): beg(beg), end(end) {}

    Slice (int rA, int cA, int rB, int cB): beg(Point(rA, cA)), 
        end(Point(rB, cB)) {}

    Slice () {}

    void cut (Pizza & pizza);

    bool is_valid (Pizza & pizza, int & t, int & m, int & e);

    bool is_invalid_without_hope (Pizza & pizza);

    bool is_invalid_with_hope (Pizza & pizza) {
        int t, m, e;
        if (is_valid(pizza, t, m, e)) {
            return false;
        }

        // If the ingredients are less than or equal to the minimum then we have hope
        if (t <= L && m <= L) {
            return true;
        }
        return false;
    }

    void print_output () {
        beg.print_output();
        end.print_output();
    }

    bool can_add_col (Pizza & pizza);

    bool can_add_row (Pizza & pizza);

    void add_row () { 
        // cerr << "Adding Row\n";
        ++end.r; 
    }

    void add_col () { 
        // cerr << "Adding Column\n";
        ++end.c;
    }

    void remove_row () { 
        // cerr << "Removing Row\n";
        --end.r; 
    }

    void remove_col () { 
        // cerr << "Removing Column\n";
        --end.c; 
    }
};

class Pizza {
public:
    vector<vector <char> > grid;
    vector<Slice> slices;
    const int rows;
    const int cols; 

    Pizza (int rows, int cols, const vector<string> & input_lines): 
            rows(rows), cols(cols) {

        init_grid(rows, cols);
        read_pizza_input(input_lines);
    }

    int compute_score () {
        int score = 0;
        for (int r = 0; r < this->rows; ++r) {
            for (int c = 0; c < this->cols; ++c) {
                if (grid[r][c] == EMPTY) {
                    ++score;
                }
            }
        }
        return score;
    }

    void init_grid (int rows, int cols) {
        this->grid = vector< vector<char> >(rows);
        for (int r = 0; r < rows; ++r) {
            this->grid[r] = vector<char>(cols);
        }
    }

    void read_pizza_input (const vector<string> & input_lines) {
        assert(input_lines.size() == this->rows);
        for (int r = 0; r < this->rows; ++r) {
            for (int c = 0; c < this->cols; ++c) {
                grid[r][c] = input_lines[r][c];
            }
        }
    }

    void log_pizza () {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                cerr << grid[r][c] << " ";
            }
            cerr << "\n";
        }
    }

    void print_output () {
        cout << slices.size() << "\n";
        for (size_t s = 0, siz = slices.size(); s < siz; ++s) {
            slices[s].print_output();
            cout << "\n";
        }
    }

    void solve () {
        for (int r = 0; r < this->rows; ++r) {
            for (int c = 0; c < this->cols; ++c) {
                if (grid[r][c] != EMPTY) {
                    // cerr << "r = " << r << ", c = " << c << "\n";
                    Slice s(r, c, r + 1, c + 1);
                    int t, m, e;
                    // Check if the current slice is not valid 
                    // but the lowest number for mushrooms or tomatoes has not been reached
                    // But break the loop if we've exceeded the allowed number of ingredients
                    // Or we have an empty cell
                    bool gotten_slice = false;
                    while (s.is_invalid_with_hope(*this)) {
                        int _t, _m, _e;

                        // Add a row
                        if (s.can_add_row(*this)) {
                            s.add_row();
                            if (s.is_valid(*this, _t, _m, _e)) {
                                gotten_slice = true;
                                break;
                            }
                            else if (s.is_invalid_with_hope(*this)) {
                                continue;
                            }
                            else if (s.is_invalid_without_hope(*this)) {
                                // Backtrack
                                s.remove_row();
                            }
                        }
                        // Add a column
                        if (s.can_add_col(*this)) {
                            s.add_col();
                            if (s.is_valid(*this, _t, _m, _e)) {
                                gotten_slice = true;
                                break;
                            }
                            else if (s.is_invalid_with_hope(*this)) {
                                continue;
                            }
                            else if (s.is_invalid_without_hope(*this)) {
                                // Backtrack
                                s.remove_col();
                            }
                        }

                        // Invalid without hope in row or column advance
                        break;
                    }

                    // We have a valid slice, now cut it from the pizza and 
                    // add it to our list of slices
                    if (s.is_valid(*this, t, m, e)) {
                        // cerr << "We have a valid slice, now cut it from the pizza\n";
                        s.cut(*this);
                        // this->log_pizza();
                        slices.push_back(s);
                    }
                }
            }
        }
    }
};


bool Slice::can_add_col (Pizza & pizza) {
    return (end.c <= pizza.cols);
}

bool Slice::can_add_row (Pizza & pizza) {
    return (end.r <= pizza.rows);
}

// Should this be inside the Pizza class?
void Slice::cut (Pizza & pizza) {
    // cerr << "Cutting slice, " << beg.r << ", " << beg.c << " to " << end.r << ", " << end.c << "\n";
    for (int r = beg.r; r < end.r; ++r) {
        for (int c = beg.c; c < end.c; ++c) {
            pizza.grid[r][c] = EMPTY;
        }
    }
}

bool Slice::is_invalid_without_hope (Pizza & pizza) {
    int t, m, e;
    if (is_valid(pizza, t, m, e)) {
        return false;
    }

    if ((t + m > H) /* Too many ingredients */ || (e =! 0) /* Empty cell */) {
        return true;
    } 
    /* Out of bounds */
    if (end.r > pizza.rows || end.c > pizza.cols) {
        return true;
    }
    return false;
}

bool Slice::is_valid (Pizza & pizza, int & _t, int & _m, int & _e) {
    // Count tomatoes & mushrooms
    int t = 0, m = 0, e = 0;
    // // cerr << "is_valid, " << beg.r << ", " << beg.c << " to " << end.r << ", " << end.c << "\n";
    for (int r = beg.r; r < end.r && end.r <= pizza.rows; ++r) {
        for (int c = beg.c; c < end.c && end.c <= pizza.cols; ++c) {
            // Tomato
            if (pizza.grid[r][c] == TOMATO) {
                ++t;
            }
            // Mushroom
            else if (pizza.grid[r][c] == MUSHROOM) {
                ++m;
            }
            // Empty cell
            else if (pizza.grid[r][c] == EMPTY) {
                ++e;
            }
            else {
                ostringstream oss;
                oss << "Invalid grid element at " << r << "," << c << ". Expected M, T, or " << EMPTY;
                throw oss.str();
            }
        }
    }

    _t = t;
    _m = m;
    _e = e;

    /* Out of bounds */
    if (end.r > pizza.rows || end.c > pizza.cols) {
        // cerr << "Added row/col goes out of bounds\n";
        return false;
    }

    if (e > 0) {
        // cerr << "One or more cells are empty\n";
        return false;
    }

    // Number of tomatoes or mushrooms is less than the min number
    if (t < L || m < L) {
        // cerr << "The number of tomatoes or mushrooms is less than the min number, t = " << t << ", m = " << m << ", L = " << L << "\n";
        return false;
    }

    // Total number of cells is greater than the max number
    if (t + m > H) {
        // cerr << "The total number of cells is greater than the max number\n";
        return false;
    }
    return true;
}

int main () {
    cin.tie(NULL);
    ios::sync_with_stdio(false);

    cin >> R >> C >> L >> H;
    // cerr << "R = " << R << ", C = " << C << ", L = " << L << ", H = " << H << "\n";
    cin.ignore();
    vector<string> input_lines(R);
    string line;
    for (int r = 0; r < R; ++r) {
        getline(cin, line);
        input_lines[r] = line;
    }

    Pizza pizza(R, C, input_lines);

    pizza.solve();
    pizza.log_pizza();
    pizza.print_output();

    cerr << "\nScore: " << pizza.compute_score() << "\n";

    return EXIT_SUCCESS;
}