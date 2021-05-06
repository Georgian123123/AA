// Copyright 2020
// Authors: Radu Nichita, Matei Simtinică

#ifndef TASK2_H_
#define TASK2_H_

#include "task.h"

/*
 * Task2
 * You have to implement 4 methods:
 * read_problem_data         - read the problem input and store it however you see fit
 * formulate_oracle_question - transform the current problem instance into a SAT instance and write the oracle input
 * decipher_oracle_answer    - transform the SAT answer back to the current problem's answer
 * write_answer              - write the current problem's answer
 */
class Task2 : public Task {
 private:
    std::vector<std::vector<int>> adj;
    int N;
    int M;
    int K;
    int V;
    std::string oracle_answer;
    std::vector<int> answers;
 public:
    void solve() override {
        read_problem_data();
        formulate_oracle_question();
        ask_oracle();
        decipher_oracle_answer();
        write_answer();
    }

    void read_problem_data() override {
        /* Vom reprezenta datele sub forma unui graf,
         *reprezentat prin matrice de adiacenta.
         */
        std::ifstream in_file(in_filename);
        in_file >> N >> M >> K;
        adj = std::vector<std::vector<int>>(N + 1, std::vector<int>(N + 1));
        for (int i = 0; i < M; i++) {
            int u, v;
            in_file >> u >> v;
            adj[u][v] = 1;
            adj[v][u] = 1;
        }
        in_file.close();
    }

    void formulate_oracle_question() {
        /* 
         * Problema care trebuie redusa la probela SAT este k-clica.
         * 
         * Variabilele pe care le trimitem oracolului au urmatoarea
         * insemnatate: nodului j ii este asociat indexul i in
         * clica desemnata de literal.
         * 
         */
        std::ofstream oracle_in_file(oracle_in_filename);
        int literal = 1;
        std::unordered_map<int, std::pair<int, int>> literal_dict;
        std::vector<std::string> clauses;

        /* Pentru noduri
         * Ne asiguram ca fiecare nod are un index intr-o clica
         */
        for (int i = 1; i <= K; i++) {
            std::string clause = "";
            for (int j = 1; j <= N; j++) {
                literal_dict[literal] = std::make_pair(i, j);
                clause += std::to_string(literal) + " ";
                literal++;
            }
            clause += "0";
            clauses.push_back(clause);
        }

        /* Pentru muchii
         *
         * Cand nu este muchie intre noduri diferite: 
         *      ne asiguram ca acele noduri sa nu fie parte din aceiasi clica
         * 
         * Cand este muchie intre noduri:
         *      ne asiguram ca doua noduri sa nu aibe acelasi indice intr-o
         *      clica si ca un nod sa nu aibe 2 indici diferiti in clica
         */
        for (auto literal1 : literal_dict) {
            int i = literal_dict[literal1.first].first;
            int v = literal_dict[literal1.first].second;

            for (auto literal2 : literal_dict) {
                int j = literal_dict[literal2.first].first;
                int w = literal_dict[literal2.first].second;

                if (adj[v][w] == 0 && v != w &&
                        literal1.first != literal2.first) {
                    std::string clause = "-" + std::to_string(literal1.first)
                        + " -" + std::to_string(literal2.first) + " 0";
                    clauses.push_back(clause);
                }
                
                if (((v == w && i != j) || (v != w && i == j)) &&
                        literal1.first != literal2.first) {
                    std::string clause = "-" + std::to_string(literal1.first)
                        + " -" + std::to_string(literal2.first) + " 0";
                    clauses.push_back(clause);
                }
            }
        }
        
        oracle_in_file << "p cnf " << literal_dict.size()
            << " " << clauses.size() << std::endl;
        for (auto clause : clauses) {
            oracle_in_file << clause << std::endl;
        }
        oracle_in_file.close();
    }

    void decipher_oracle_answer() {
        std::ifstream oracle_out_file(oracle_out_filename);
        oracle_out_file >> oracle_answer;
        oracle_out_file >> V;
        for (int i = 0; i < V; i++) {
            int aux;
            oracle_out_file >> aux;
            answers.push_back(aux);
        }
        oracle_out_file.close();
    }

    void write_answer() override {
        /* Din ce intoarce oracolul, ne intereseaza
         * doar variabilele pozitive. Ele reprezinta
         * variabilele asociate nodurilor dintr-o clica.
         * vom tine cont carui nod ii este asociata o
         * variabila printr-un contor.
         */
        std::ofstream out_file(out_filename);
        out_file << oracle_answer << std::endl;
        int contor = 1;
        for (int i = 0; i < answers.size(); ++i) {
            if (answers[i] > 0) {
                out_file <<  contor << " ";
            }
            contor++;
            if (contor == N + 1) {
                contor = 1;
            }
        }
        out_file.close();
    }
};

#endif  // TASK2_H_
