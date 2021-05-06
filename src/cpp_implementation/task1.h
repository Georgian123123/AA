// Copyright 2020
// Authors: Radu Nichita, Matei Simtinică

#ifndef TASK1_H_
#define TASK1_H_

#include "task.h"

/*
 * Task1
 * You have to implement 4 methods:
 * read_problem_data         - read the problem input and store it however you see fit
 * formulate_oracle_question - transform the current problem instance into a SAT instance and write the oracle input
 * decipher_oracle_answer    - transform the SAT answer back to the current problem's answer
 * write_answer              - write the current problem's answer
 */
class Task1 : public Task {
 private:

    std::unordered_map<int, std::vector<int>> input;
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
         * reprezentat prin lista de adiacenta.
         */
        std::ifstream in_file(in_filename);
        in_file >> N >> M >> K;
        for (int i = 0; i < M; i++) {
            int u, v;
            in_file >> u >> v;
            input[u].push_back(v);
            input[v].push_back(u);
        }
        in_file.close();
    }

    /* Functie care face conversia unei perechi
     * nod/culoare intr-o variabila asociata
     */
    int translate_to_variable(int node, int color)
    {
        return (node - 1) * K + color
    }

    /* Functie care face conversia unei variabile asociate intr-o
     * pereche nod/culoare intr-o variabila asociata
     */
    std::pair<int , int> translate_from_variable(int variable)
    {
        std::pair<int, int> res;
        res.second = variable % K;
        if (res.second == 0)
            res.second = K;

        res.first = (variable - res.second) / K + 1;
        return res;
    }

    void formulate_oracle_question() {
        /* 
         * Problema care trebuie redusa la probela SAT este k-colorare.
         * 
         * Variabilele pe care le trimitem oracolului au urmatoarea
         * insemnatate: nodului i ii va fi asociata culoarea j.
         * 
         */
        int clause_counter = 0;

        std::ofstream oracle_in_file(oracle_in_filename);
        std::vector<std::vector<int>> rules;

        /* Pentru Noduri
         * Ne asiguram ca fiecarui nod ii este asociata
         * o culoare si numai una.
         */
        for (int i = 1; i <= N; i ++) {
            std::vector<int> new_rule;
            for (int j = 1; j <= K; j++) {
                new_rule.push_back(translate_to_variable(i, j));
            }
            rules.push_back(new_rule);
            clause_counter++;
        }
        
        for (int i = 1; i <= N; i ++) {
            for (int j = 1; j <= K; j++) {
                for(int p = 1; p <= K; p++) {
                    if (j != p) {
                        std::vector<int> new_rule;
                        new_rule.push_back(-translate_to_variable(i, j));
                        new_rule.push_back(-translate_to_variable(i, p));
                        clause_counter++;
                        rules.push_back(new_rule);
                    }
                }
            }
        }

        /* Pentru muchii 
         * Ne asiguram ca 2 noduri adiacente nu pot lua aceiasi culoare
         */
        int u;
        for (auto edge : input) {
            u = edge.first;
            for  (auto v : edge.second) {      
                for (int i = 1; i <= K; i++) {
                    std::vector<int> new_rule;
                    new_rule.push_back(-translate_to_variable(u, i));
                    new_rule.push_back(-translate_to_variable(v, i));
                    clause_counter++;
                    rules.push_back(new_rule);
                } 
            }
        }
  

        oracle_in_file << "p cnf " << K * N << " " << clause_counter << std::endl;
        for( auto rule : rules ) {
            for (auto var : rule) {
                oracle_in_file << var << " ";
            }
            oracle_in_file << "0\n";
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
        /* Din ce intoare oracolul ne intereseaza variabilele pozitive
         * In functie de acele variabile vom afisa nodurile si culorile
         * asociate lor.
         */
        std::vector<std::pair<int, int>> res_arr;
        std::ofstream out_file(out_filename);
        out_file << oracle_answer << std::endl;
        for (auto number : answers) {
            if (number > 0) {
                std::pair<int, int> res = translate_from_variable(number);
                res_arr.push_back(res);
            }
        }

        //std::sort(res_arr.begin(), res_arr.end());
        for (auto el : res_arr) {
            out_file << el.second << " ";
        }
        out_file.close();

    }
};

#endif  // TASK1_H_
