#include <iostream>
#include <map>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <string>
using namespace std;

struct Vertex {
    multimap<int, Vertex*> adjacency_list;
    string oligo_nucleotide;
};

struct Input {
    string dna;
    int length = 12;
    int k = 4;
    int delta = 1;
    int positive_error = 1;
    int negative_error = 1;
    bool probable_positive_error = false;
    bool rep_allowed = true;
};

Input base_input;
Input input;

//Mapa odpowiadająca za nasz graf
unordered_map<string, Vertex> graph;
Vertex* Head;

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



// O(k*n^2)
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
                if(it1->first.substr(0, size - 1).find(it2->first) != std::string::npos){
                    //indeks do ustalenia
                    it1->second.adjacency_list.insert(make_pair(0, &it2->second));
                }
            }
        }
    }
}

void Menu() {
    string u_input;
    string temp_input;
    int i_input;

    cout << "WYBIERZ OPCJE:" << endl;
    cout << "1. Generator instancji" << endl;
    cout << "2. Algorytm Naiwny" << endl;
    cout << "3. Metaheurystyka" << endl;

    cin >> i_input;
    cin.ignore(); // Oczyszczenie bufora po wprowadzeniu pierwszej opcji

    switch (i_input) {
        case 1: {
            cout << "WYBIERZ OPCJE GENERATORA INSTANCJI:" << endl;
            cout << "1. Wczytaj z pliku" << endl;
            cout << "2. Generuj recznie" << endl;

            cin >> i_input;
            cin.ignore(); // Oczyszczenie bufora po wprowadzeniu drugiej opcji

            switch (i_input) {
                case 1:
                    cout << "Wczytaj DNA z pliku" << endl;
                    // Kod do wczytywania DNA z pliku
                    cout << "WCZYTANO Z PLIKU" << endl;
                    break;

                case 2: {
                    cout << "Generuj recznie" << endl;

                    cout << "Dlugosc DNA:" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.length = stoi(temp_input);
                    }

                    cout << "Dlugosc podciagow DNA:" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.k = stoi(temp_input);
                    }

                    cout << "Wartosc delta_k:" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.delta = stoi(temp_input);
                    }

                    cout << "Czy maja byc bledy negatywne wynikajace z powtorzen? Tak/Nie" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.rep_allowed = (temp_input == "Tak");
                    }

                    cout << "Ile ma byc bledow negatywnych nie wynikajacych z powtorzen?" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.negative_error = stoi(temp_input);
                    }

                    cout << "Ile ma byc bledow pozytywnych?" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.positive_error = stoi(temp_input);
                    }

                    cout << "Czy bledy pozytywne maja byc prawdopodobne? Tak/Nie" << endl;
                    getline(cin, temp_input); // Odczyt danych
                    if (!temp_input.empty()) {
                        input.probable_positive_error = (temp_input == "Tak");
                    }

                    graph = generateSpectrum();
                    break;
                }

                default:
                    cout << "Nieprawidlowa opcja." << endl;
                    break;
            }
            break;
        }

        case 2:
            cout << "Algorytm Naiwny" << endl;
            // Kod dla algorytmu naiwnego
            break;

        case 3:
            cout << "Metaheurystyka" << endl;
            // Kod dla metaheurystyki
            break;

        default:
            cout << "Nieprawidlowa opcja" << endl;
            break;
    }
}





int main() {
    srand(time(nullptr));
    Menu();
    printSpectrum();
    connectGraph();
    cout << endl << endl;
    for(unordered_map<string, Vertex>::iterator i = graph.begin(); i != graph.end(); i++){
        cout << i->first << " => ";
        for(multimap<int, Vertex*>::iterator ii = i->second.adjacency_list.begin(); ii != i->second.adjacency_list.end(); ii++){
            cout << ii->second->oligo_nucleotide << "|" << ii->first << "   ";
        }
        cout << endl;
    }
}
//CONNECT GRAPH
//PROPABLE ERROR
//RAND() wymienic
