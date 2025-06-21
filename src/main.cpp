// -------------------- ИМПОРТЫ И ОБЩИЕ СТРУКТУРЫ --------------------
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

using namespace std;
using namespace chrono;

struct Instance {
    int capacity;
    vector<int> weights;
    vector<int> profits;
    vector<int> optimal;
};

Instance load_instance(const string &prefix) {
    Instance inst;
    string data_path = "src/data/" + prefix;
    ifstream fcap(data_path + "_c.txt");
    fcap >> inst.capacity;

    ifstream fw(data_path + "_w.txt");
    inst.weights = vector<int>(istream_iterator<int>(fw), {});

    ifstream fp(data_path + "_p.txt");
    inst.profits = vector<int>(istream_iterator<int>(fp), {});

    ifstream fs(data_path + "_s.txt");
    inst.optimal = vector<int>(istream_iterator<int>(fs), {});

    return inst;
}

int match(const vector<int> &sol, const vector<int> &opt) {
    int correct = 0;
    for (size_t i = 0; i < sol.size(); ++i)
        if (sol[i] == opt[i]) ++correct;
    return static_cast<int>(100.0 * correct / sol.size());
}

// -------------------- 2-АППРОКСИМАЦИЯ --------------------
vector<int> two_approx(const Instance &inst, int &total_weight, int &total_profit) {
    int n = inst.weights.size();
    vector<pair<double, int>> ratio;
    for (int i = 0; i < n; ++i) ratio.emplace_back((double)inst.profits[i] / inst.weights[i], i);
    sort(ratio.rbegin(), ratio.rend());

    vector<int> taken(n, 0);
    total_weight = 0;
    total_profit = 0;

    for (auto &[_, i] : ratio) {
        if (total_weight + inst.weights[i] <= inst.capacity) {
            taken[i] = 1;
            total_weight += inst.weights[i];
            total_profit += inst.profits[i];
        }
    }
    return taken;
}

// -------------------- ДИНАМИКА ПО ВЕСУ --------------------
vector<int> dp_weights(const Instance &inst, int &total_weight, int &total_profit) {
    int n = inst.weights.size(), cap = inst.capacity;
    vector<vector<int>> dp(n + 1, vector<int>(cap + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int c = 0; c <= cap; ++c) {
            if (inst.weights[i - 1] <= c)
                dp[i][c] = max(dp[i - 1][c], dp[i - 1][c - inst.weights[i - 1]] + inst.profits[i - 1]);
            else
                dp[i][c] = dp[i - 1][c];
        }
    }
    vector<int> taken(n, 0);
    int c = cap;
    for (int i = n; i >= 1; --i) {
        if (dp[i][c] != dp[i - 1][c]) {
            taken[i - 1] = 1;
            c -= inst.weights[i - 1];
        }
    }
    total_weight = total_profit = 0;
    for (int i = 0; i < n; ++i) {
        if (taken[i]) {
            total_weight += inst.weights[i];
            total_profit += inst.profits[i];
        }
    }
    return taken;
}

// -------------------- FPTAS --------------------
vector<int> fptas(const Instance &inst, int &total_weight, int &total_profit, double epsilon = 0.1) {
    int n = inst.weights.size();
    double K = epsilon * (*max_element(inst.profits.begin(), inst.profits.end())) / n;
    vector<int> scaled(n);
    for (int i = 0; i < n; ++i) scaled[i] = (int)(inst.profits[i] / K);

    int P = accumulate(scaled.begin(), scaled.end(), 0);
    vector<int> dp(P + 1, 1e9);
    vector<int> back(P + 1, -1);
    dp[0] = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = P; j >= scaled[i]; --j) {
            if (dp[j - scaled[i]] + inst.weights[i] < dp[j]) {
                dp[j] = dp[j - scaled[i]] + inst.weights[i];
                back[j] = i;
            }
        }
    }
    int best = 0;
    for (int j = P; j >= 0; --j)
        if (dp[j] <= inst.capacity) {
            best = j;
            break;
        }

    vector<int> taken(n, 0);
    while (best > 0 && back[best] != -1) {
        int i = back[best];
        taken[i] = 1;
        best -= scaled[i];
    }
    total_weight = total_profit = 0;
    for (int i = 0; i < n; ++i)
        if (taken[i]) {
            total_weight += inst.weights[i];
            total_profit += inst.profits[i];
        }
    return taken;
}

// -------------------- BRANCH AND BOUND (с начальной оценкой) --------------------
vector<int> branch_and_bound(const Instance &inst, int &total_weight, int &total_profit) {
    struct Node {
        int level, profit, weight;
        double bound;
        vector<int> taken;
        bool operator<(const Node &other) const { return bound < other.bound; }
    };

    int n = inst.weights.size();
    vector<int> best(n, 0);
    total_profit = 0;

    auto bound = [&](const Node &u) {
        if (u.weight >= inst.capacity) return 0.0;
        double profit_bound = u.profit;
        int j = u.level;
        int totweight = u.weight;
        while (j < n && totweight + inst.weights[j] <= inst.capacity) {
            totweight += inst.weights[j];
            profit_bound += inst.profits[j];
            j++;
        }
        if (j < n) profit_bound += (inst.capacity - totweight) * ((double)inst.profits[j] / inst.weights[j]);
        return profit_bound;
    };

    vector<Node> q;
    Node u, v;
    v.level = 0;
    v.profit = 0;
    v.weight = 0;
    v.taken = vector<int>(n, 0);
    v.bound = bound(v);
    q.push_back(v);

    while (!q.empty()) {
        pop_heap(q.begin(), q.end());
        v = q.back();
        q.pop_back();

        if (v.bound <= total_profit || v.level >= n) continue;

        u.level = v.level + 1;
        u.weight = v.weight + inst.weights[v.level];
        u.profit = v.profit + inst.profits[v.level];
        u.taken = v.taken;
        u.taken[v.level] = 1;

        if (u.weight <= inst.capacity && u.profit > total_profit) {
            total_profit = u.profit;
            best = u.taken;
        }

        u.bound = bound(u);
        if (u.bound > total_profit) {
            q.push_back(u);
            push_heap(q.begin(), q.end());
        }

        u.weight = v.weight;
        u.profit = v.profit;
        u.taken = v.taken;
        u.taken[v.level] = 0;
        u.bound = bound(u);

        if (u.bound > total_profit) {
            q.push_back(u);
            push_heap(q.begin(), q.end());
        }
    }

    total_weight = 0;
    for (int i = 0; i < n; ++i)
        if (best[i]) total_weight += inst.weights[i];

    return best;
}

// -------------------- ГЕНЕТИЧЕСКИЙ АЛГОРИТМ --------------------
vector<int> genetic_algorithm(const Instance &inst, int &total_weight, int &total_profit) {
    int n = inst.weights.size();
    int population_size = 100, generations = 500;
    double crossover_prob = 0.8, mutation_prob = 0.02;
    mt19937 rng(random_device{}());
    uniform_real_distribution<double> prob(0.0, 1.0);
    uniform_int_distribution<int> bit(0, 1);

    vector<vector<int>> population(population_size, vector<int>(n));
    for (auto &ind : population)
        for (int &gene : ind) gene = bit(rng);

    auto fitness = [&](const vector<int> &ind) {
        int w = 0, p = 0;
        for (int i = 0; i < n; ++i)
            if (ind[i]) {
                w += inst.weights[i];
                p += inst.profits[i];
            }
        return (w <= inst.capacity) ? p : 0;
    };

    vector<int> best_ind(n);
    int best_fit = 0;

    for (int gen = 0; gen < generations; ++gen) {
        vector<pair<int, vector<int>>> fits;
        for (auto &ind : population) fits.emplace_back(fitness(ind), ind);
        sort(fits.rbegin(), fits.rend());

        if (fits[0].first > best_fit) {
            best_fit = fits[0].first;
            best_ind = fits[0].second;
        }

        vector<vector<int>> new_pop;
        while (new_pop.size() < population_size) {
            const vector<int> &p1 = fits[rand() % 50].second;
            const vector<int> &p2 = fits[rand() % 50].second;
            vector<int> c1 = p1, c2 = p2;
            if (prob(rng) < crossover_prob) {
                int point = rand() % n;
                for (int i = point; i < n; ++i) swap(c1[i], c2[i]);
            }
            for (int i = 0; i < n; ++i)
                if (prob(rng) < mutation_prob) c1[i] = 1 - c1[i];
            for (int i = 0; i < n; ++i)
                if (prob(rng) < mutation_prob) c2[i] = 1 - c2[i];
            new_pop.push_back(c1);
            if (new_pop.size() < population_size) new_pop.push_back(c2);
        }
        population = move(new_pop);
    }

    total_weight = total_profit = 0;
    for (int i = 0; i < n; ++i)
        if (best_ind[i]) {
            total_weight += inst.weights[i];
            total_profit += inst.profits[i];
        }
    return best_ind;
}

vector<int> fptas_wrapper(const Instance &inst, int &w, int &p) { return fptas(inst, w, p, 0.1); }

// -------------------- MAIN И CSV ВЫВОД --------------------
int main() {
    ofstream fout("src/results.csv");
    fout << "Dataset,Algorithm,Time(us),Profit,Weight,Match (%)\n";

    for (int i = 1; i <= 8; ++i) {
        string name = "p0" + to_string(i);
        Instance inst = load_instance(name);

        using Func = function<vector<int>(const Instance &, int &, int &)>;
        vector<pair<string, Func>> methods;
        methods.emplace_back("2-Approx", two_approx);
        methods.emplace_back("DP", dp_weights);
        methods.emplace_back("FPTAS", fptas_wrapper);
        methods.emplace_back("BnB", branch_and_bound);
        methods.emplace_back("GA", genetic_algorithm);

        for (auto &[label, func] : methods) {
            int w = 0, p = 0;
            auto start = high_resolution_clock::now();
            vector<int> sol = func(inst, w, p);
            auto end = high_resolution_clock::now();
            double time = duration_cast<microseconds>(end - start).count();
            int acc = match(sol, inst.optimal);
            fout << name << "," << label << "," << time << "," << p << "," << w << "," << acc << "\n";
        }
    }

    fout.close();
    cout << "Results written to results.csv" << endl;
    return 0;
}
