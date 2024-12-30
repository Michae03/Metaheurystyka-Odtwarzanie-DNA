#include <iostream>
#include <map>
#include <unordered_map>
#include <random>
#include <fstream>
#include <string>

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

bool generate_spectrum(unordered_map<string, Vertex> &spectrum, Input &input){
    input.dna = generate_dna(input.length);
    int offset;
    string new_oligo = "";
    //GENEROWANE SPEKTRUM BEZBŁĘDNE
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
                return false;
            }
        }
        spectrum.insert(make_pair(new_oligo, new_vertex));
        new_oligo = "";
    }
    return true;
}

void generate_negative(unordered_map<string, Vertex> &spectrum, Input &input){
    for(int i = 0; i < input.negative_error; i++) {
        int random_index = rand() % int(spectrum.size());
        auto it = spectrum.begin();
        advance(it, random_index);
        //cout << "NEG: " << it->first << "\n";
        spectrum.erase(it);
    }
}

void generate_probable(unordered_map<string, Vertex> &spectrum, unordered_map<string, Vertex>::iterator it, int place){
    // GENERUJEMY I SPRAWDZAMY CZY ZNAK SIE ZMIENIŁ
    string new_oligo;
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
}

void generate_positive(unordered_map<string, Vertex> &spectrum, Input &input){
    string new_oligo;
    int holder;
    for(int i = 0; i < input.positive_error; i++){
        if(!input.probable_positive_error) {
            holder = rand() % (2*input.delta + 1) - input.delta;
            Vertex new_vertex;
            new_oligo = generate_dna(input.k + holder);
            while(spectrum.contains(new_oligo)) {
                new_oligo = generate_dna(input.k + holder);
            }
            new_vertex.oligo_nucleotide = new_oligo;
            spectrum.insert(make_pair(new_vertex.oligo_nucleotide, new_vertex));
        }
        else{
            holder = rand() % int(spectrum.size());
            unordered_map<string, Vertex>::iterator it = spectrum.begin();
            advance(it, holder);
            holder = it->first.size() - 1;
            generate_probable(spectrum, it, holder / 2);
            generate_probable(spectrum, it, holder);
            i++;
        }
    }
}

unordered_map<string, Vertex> generateInstance(Input &input) {
    unordered_map<string, Vertex> spectrum, graph;
    bool valid_spectrum;
    int i = 0;
    do{
        valid_spectrum = generate_spectrum(spectrum, input);

    } while(valid_spectrum == false || i > 10000, i++);
    if(input.negative_error) generate_negative(spectrum, input);
    if(input.positive_error) generate_positive(spectrum, input);
    return spectrum;
}

void print_spectrum(){
    int i = 1;
    for(auto &it : graph) {
        cout<< i <<": " << it.first << endl;
        i++;
    }
}

void read_from_file(){
    //fstream inFile("instancja.txt");
    
}

void connectGraph(){
    int size;
    //int num;
    for(unordered_map<string, Vertex>::iterator it1 = graph.begin(); it1 != graph.end(); it1++){
        for(unordered_map<string, Vertex>::iterator it2 = graph.begin(); it2 != graph.end(); it2++){
            if(it1 != it2){
                size = it1->first.size();
                //num = size;
                //if(num > it2->first.size()) num = it2->first.size();
                for(int i = 0; i < size; i++){
                    if(it1->first.substr(size - i - 1, i + 1) == it2->first.substr(0, i + 1)){
                        it1->second.adjacency_list.insert(make_pair(i + 1, &it2->second));
                    }
                }  
                if(it1->first.substr(0, size - 1).contains(it2->first)){
                    //indeks do ustalenia
                    it1->second.adjacency_list.insert(make_pair(0, &it2->second));
                } 
            }
        }
    }
}

int main() {
srand(time(nullptr));
    //unordered_map<string, Vertex> spectrum;
    Input input;
    input.length = 10;
    input.k = 4;
    input.delta = 2;
    input.positive_error = 0;
    input.negative_error = 0;
    input.probable_positive_error = false;
    input.rep_allowed = true;
    graph = generateInstance(input);
    connectGraph();
    cout<<input.dna<<endl;
    print_spectrum();
    cout << endl << endl;
    for(unordered_map<string, Vertex>::iterator i = graph.begin(); i != graph.end(); i++){
        cout << i->first << " => ";
        for(multimap<int, Vertex*>::iterator ii = i->second.adjacency_list.begin(); ii != i->second.adjacency_list.end(); ii++){
            cout << ii->second->oligo_nucleotide << "|" << ii->first << "   ";
        }
        cout << endl;
    }
}