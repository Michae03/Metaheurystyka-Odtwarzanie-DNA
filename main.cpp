#include <iostream>
#include <map>
#include <unordered_map>
#include <random>
#include <algorithm>

using namespace std;

struct Vertex {
    multimap<int, Vertex*> adjacency_list;
    string oligo_nucleotide;
};

struct Input {
    string dna;
    int length = 400;
    int k = 8;
    int delta = 2;
    int positive_error = 0;
    int negative_error = 0;
    bool probable_positive_error = false;
    bool rep_allowed = true;
};

Input input;

//Mapa odpowiadająca za nasz graf
unordered_map<string, Vertex> graph;
Vertex Head;

string generateDNA(int length) {
    string dna;
    char tab_nucleotides[4] = {'A', 'C', 'G', 'T'};
    for(int i = 0; i < length; i++) {
        dna += tab_nucleotides[rand() % 4];
    }
    return dna;
}

void randomize_spectrum(unordered_map<string, Vertex> &spectrum) {
    std::vector<std::pair<std::string, Vertex>> vec(spectrum.begin(), spectrum.end());
    random_device rd;
    mt19937 g(rd());
    shuffle(vec.begin(), vec.end(), g);
    spectrum.clear();
    for (const auto &pair : vec) {
        spectrum.insert(pair);
    }
}

unordered_map<string, Vertex> generate_flawless_spectrum() {
    unordered_map<string, Vertex> flawless_spectrum;
    unordered_map<string, Vertex> ERROR;
    ERROR.insert(make_pair("ERROR", Vertex()));
    int offset;
    string new_oligo;
    bool negative_error_detected = false;
    for(int i = 0; i < input.length - input.k + 1; i++) {

        if(i == 0 || i >= input.length - input.k - 2) {
            offset = 0;
        } else {
            offset = rand() % (2*input.delta + 1) - input.delta;
        }
        for(int j = 0; j < input.k + offset; j++) {
            new_oligo += input.dna[i + j];
        }
        Vertex new_vertex;
        new_vertex.oligo_nucleotide = new_oligo;
        if(!input.negative_error && !input.rep_allowed) {
            if(flawless_spectrum.contains(new_vertex.oligo_nucleotide)) {;
                return ERROR;
            }
        }
       flawless_spectrum.insert(make_pair(new_oligo, new_vertex));
        new_oligo = "";
    }
    return flawless_spectrum;
}

void generate_negative_error(unordered_map<string, Vertex> &spectrum) {
    for(int i = 0; i < input.negative_error; i++) {
        int random_index = rand() % int(spectrum.size());
        auto it = spectrum.begin();
        advance(it, random_index);
        spectrum.erase(it);
    }
}

void generate_positive_error(unordered_map<string, Vertex> &spectrum) {
    int offset;
    string new_oligo = "";
    for(int i = 0; i < input.positive_error; i++) {
        if(!input.probable_positive_error) {
            offset = rand() % (2*input.delta + 1) - input.delta;
            Vertex new_vertex;
            new_oligo = generateDNA(input.k + offset);
            while(spectrum.contains(new_oligo)) {
                new_oligo = generateDNA(input.k + offset);
            }
            new_vertex.oligo_nucleotide = new_oligo;
            spectrum.insert(make_pair(new_vertex.oligo_nucleotide, new_vertex));
        } else {
            // ZNAJDOWANIE LOSOWEGO WIERZCHOŁKA
            int random_index = rand() % int(spectrum.size());
            auto it = spectrum.begin();
            advance(it, random_index);
            // WYBIERANIE NUKLEOTYDU PO ŚRODKU
            int place = int(it->first.size() - 1) /2;
            // GENERUJEMY I SPRAWDZAMY CZY ZNAK SIE ZMIENIŁ
            char current_sign = it->first.at(place);
            char change = generateDNA(1)[0];
            while(change == current_sign) {
                change = generateDNA(1)[0];
            }
            // TWORZYMY NOWĄ PODSEKWENCJE
            new_oligo = it->second.oligo_nucleotide;
            new_oligo.at(place) = change;
            // DODAJEMY NOWY WIERZCHOŁEK nr1
            Vertex new_vertex1;
            new_vertex1.oligo_nucleotide = new_oligo;
            spectrum.insert(make_pair(new_oligo, new_vertex1));
            // WYBIERANIE NUKLEOTYDU NA KOŃCU
            place = int(it->first.size() - 1);
            // GENERUJEMY I SPRAWDZAMY CZY ZNAK SIE ZMIENIŁ
            current_sign = it->first.at(place);
            change = generateDNA(1)[0];
            while(change == current_sign) {
                change = generateDNA(1)[0];
            }
            // TWORZYMY NOWĄ PODSEKWENCJE
            new_oligo = it->second.oligo_nucleotide;
            new_oligo.at(place) = change;
            // DODAJEMY NOWY WIERZCHOŁEK nr1
            Vertex new_vertex2;
            new_vertex1.oligo_nucleotide = new_oligo;
            spectrum.insert(make_pair(new_oligo, new_vertex2));
            // USUWAMY WYBRAŃCA
            spectrum.erase(it->first);
            // INKEREMNTUJEMY i BO DODALIŚMY DWA BŁĘDY
            i++;
        }

    }
}

unordered_map<string, Vertex> generateSpectrum() {
    unordered_map<string, Vertex> spectrum;
    do {
        input.dna = generateDNA(input.length);
        spectrum = generate_flawless_spectrum();
    } while (spectrum.contains("ERROR"));
    generate_negative_error(spectrum);
    generate_positive_error(spectrum);
    randomize_spectrum(spectrum);
    return spectrum;
}

void printSpectrum(){
    int i = 1;
    for(auto &it : graph) {
        cout<< i <<": " << it.first << endl;
        i++;
    }
}



int main() {
srand(time(nullptr));
    graph = generateSpectrum();
    cout<<input.dna<<endl;
    printSpectrum();
}
