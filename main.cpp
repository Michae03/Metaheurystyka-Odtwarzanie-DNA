#include <iostream>
#include <map>
#include <unordered_map>
#include <random>

using namespace std;

struct Vertex {
    multimap<int, Vertex*> adjacency_list;
    string oligo_nucleotide;
};

struct Input {
    string dna;
    int length;
    int k;
    int delta;
    int positive_error;
    int negative_error;
    bool probable_positive_error;
    bool rep_allowed;
};

//Mapa odpowiadająca za nasz graf
unordered_map<string, Vertex> graph;
Vertex Head;

string generate_dna(int length) {
    string dna;
    char tab_nucleotides[4] = {'A', 'C', 'G', 'T'};
    for(int i = 0; i < length; i++) {
        dna += tab_nucleotides[rand() % 4];
    }
    return dna;
}

unordered_map<string, Vertex> generate_spectrum(const Input &input) {
    unordered_map<string, Vertex> spectrum;
    unordered_map<string, Vertex> ERROR;
    ERROR.insert(make_pair("ERROR", Vertex()));
    int offset;
    string new_oligo = "";
    //GENEROWANE SPEKTRUM BEZBŁĘDNE
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
            if(spectrum.contains(new_vertex.oligo_nucleotide)) {;
                return ERROR;
            }
        }
        spectrum.insert(make_pair(new_oligo, new_vertex));
        new_oligo = "";
    }
    //BŁĘDY NEGATYWNE
    for(int i = 0; i < input.negative_error; i++) {
        int random_index = rand() % int(spectrum.size());
        auto it = spectrum.begin();
        advance(it, random_index);
        spectrum.erase(it);
    }
    //BŁĘDY POZYTYWNE
    for(int i = 0; i < input.positive_error; i++) {
        if(!input.probable_positive_error) {
            offset = rand() % (2*input.delta + 1) - input.delta;
            Vertex new_vertex;
            new_oligo = generate_dna(input.k + offset);
            while(spectrum.contains(new_oligo)) {
                new_oligo = generate_dna(input.k + offset);
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
            char change = generate_dna(1)[0];
            while(change == current_sign) {
                change = generate_dna(1)[0];
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
            change = generate_dna(1)[0];
            while(change == current_sign) {
                change = generate_dna(1)[0];
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
            // INKEREMNTUJEMY I BO DODALIŚMY DWA BŁĘDY
            i++;
        }

    }
    return spectrum;
}

void print_spectrum(){
    int i = 1;
    for(auto &it : graph) {
        cout<< i <<": " << it.first << endl;
        i++;
    }
}

int main() {
srand(time(nullptr));
    Input input;
    input.length = 700;
    input.k = 9;
    input.delta = 2;
    input.positive_error = 0;
    input.negative_error = 0;
    input.probable_positive_error = true;
    input.rep_allowed = false;
    int TEST = 0;
    do {
        input.dna = generate_dna(input.length);
        TEST++;
        cout<<"TEST: "<<TEST<<endl;
        graph = generate_spectrum(input);
    } while(graph.contains("ERROR") || TEST > 10000);
    cout<<input.dna<<endl;
    print_spectrum();
    cout<<"TEST: "<<TEST<<endl;
}
