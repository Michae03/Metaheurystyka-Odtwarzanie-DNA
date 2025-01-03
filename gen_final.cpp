#include <iostream>
#include <map>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

struct Vertex {
    multimap<int, Vertex*> adjacency_list;
    string oligo_nucleotide;
};

struct Input {
    string dna = "";
    int n = 400;
    int k = 8;
    int delta_k = 2;
    int l_neg = 0;
    int l_poz = 0;
    bool repAllowed = true;
    bool probablePositive = false;
    string first_oli = "";
};

Input input;
Input base_input;

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

unordered_map<string, Vertex> generate_spectrum() {
    unordered_map<string, Vertex> flawless_spectrum;
    unordered_map<string, Vertex> ERROR;
    ERROR.insert(make_pair("ERROR", Vertex()));
    int offset;
    string new_oligo;
    bool negative_error_detected = false;
    for(int i = 0; i < input.n - input.k + 1; i++) {
        if(i == 0 || i >= input.n - input.k - 2) {
            offset = 0;
        } else {
            offset = rand() % (2*input.delta_k + 1) - input.delta_k;
        }
        for(int j = 0; j < input.k + offset; j++) {
            new_oligo += input.dna[i + j];
        }
        Vertex new_vertex;
        new_vertex.oligo_nucleotide = new_oligo;
        if(!input.l_neg && !input.repAllowed) {
            if(flawless_spectrum.contains(new_vertex.oligo_nucleotide)) {;
                return ERROR;
            }
        }
       flawless_spectrum.insert(make_pair(new_oligo, new_vertex));
       if(i == 0) input.first_oli = new_oligo;
        new_oligo = "";
    }
    return flawless_spectrum;
}

void generate_negative_error(unordered_map<string, Vertex> &spectrum) {
    for(int i = 0; i < input.l_neg; i++) {
        int random_index = rand() % int(spectrum.size());
        auto it = spectrum.begin();
        advance(it, random_index);
        spectrum.erase(it);
    }
}

void generate_probable(unordered_map<string, Vertex> &spectrum, unordered_map<string, Vertex>::iterator it, int place){
    // GENERUJEMY I SPRAWDZAMY CZY ZNAK SIE ZMIENIŁ
    string new_oligo;
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
}

void generate_positive_error(unordered_map<string, Vertex> &spectrum){
    string new_oligo;
    int holder;
    for(int i = 0; i < input.l_poz; i++){
        if(!input.probablePositive) {
            holder = rand() % (2*input.delta_k + 1) - input.delta_k;
            Vertex new_vertex;
            new_oligo = generateDNA(input.k + holder);
            while(spectrum.contains(new_oligo)) {
                new_oligo = generateDNA(input.k + holder);
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

unordered_map<string, Vertex> generateInstance() {
    unordered_map<string, Vertex> spectrum;
    do {
        input.dna = generateDNA(input.n);
        spectrum = generate_spectrum();
    } while (spectrum.contains("ERROR"));
    if(input.l_neg) generate_negative_error(spectrum);
    if(input.l_poz) generate_positive_error(spectrum);
    randomize_spectrum(spectrum);
    return spectrum;
}

void read_from_file(string file_name){
    string seq = "";
    int spec_size;
    fstream inFile(file_name);
    if(!inFile) cout << "Nie odnaleziono pliku!\n";
    else{
        while(getline(inFile, seq) && seq[0] != '>'){
            input.dna += seq;
        }
        inFile >> spec_size;
        inFile >> input.first_oli;
        for(int i = 0; i <spec_size; i++){
            inFile >> seq;
            Vertex new_vertex;
            new_vertex.oligo_nucleotide = seq;
            graph.insert(make_pair(seq, new_vertex));
            //if(seq == first) Head = &graph[seq];
        }
        input.n = input.dna.size();
        cout << "Wczytano z pliku!\n";
    }
    inFile.close();
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
    for(unordered_map<string, Vertex>::iterator it1 = graph.begin(); it1 != graph.end(); it1++){
        if(it1->second.oligo_nucleotide == input.first_oli) Head = &it1->second;
        for(unordered_map<string, Vertex>::iterator it2 = graph.begin(); it2 != graph.end(); it2++){
            if(it1 != it2){
                size = it1->first.size();
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

void read_input(){
    string temp_input;
    cout << "Podaj n: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.n = stoi(temp_input);
    }

    cout << "Podaj k: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.k = stoi(temp_input);
    }

    cout << "Podaj delta_k: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.delta_k = stoi(temp_input);
    }

    cout << "Podaj l_neg: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.l_neg = stoi(temp_input);
    }

    cout << "Podaj l_poz: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.l_poz = stoi(temp_input);
    }

    cout << "Podaj repAllowed: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.repAllowed = (temp_input == "true");
    }

    cout << "Podaj positiveProbable: ";
    getline(cin, temp_input); // Odczyt danych
    if (!temp_input.empty()) {
        input.probablePositive = stoi(temp_input);
    }

}  

void Menu() {
    int i_input;
    while(true){
        if(i_input == 2){
            cout << "\nn: " << input.n;
            cout << "\nk: " << input.k;
            cout << "\nk_delta: " << input.delta_k;
            cout << "\nl_neg: " << input.l_neg;
            cout << "\nl_poz: " << input.l_poz;
            cout << "\nrepAllowed: " << input.repAllowed;
            cout << "\nprobablePositive: " << input.probablePositive << "\n";
        }
        else if(i_input == 1){
            cout << "\nInstancja wczytana z pliku!";
            cout << "\nn: " << input.n;
            cout << "\nSpektrum: " << graph.size() << " elem.\n";
        }

        cout << "\n~~MAIN MENIU~~\n";
        cout << "1. Generator instancji\n";
        cout << "2. Algorytm Naiwny\n";
        cout << "3. Metaheurystyka\n";
        cout << "4. Wyjdz\n";
        cout << "Wybierz opcje: ";
        cin >> i_input;
        cin.ignore(); // Oczyszczenie bufora po wprowadzeniu pierwszej opcji
        switch (i_input) {
            case 1: {
                cout << "\n~~GENERATORA INSTANCJI~~\n";
                cout << "1. Wczytaj z pliku\n";
                cout << "2. Generuj recznie\n";
                cout << "Wybierz opcje: ";
                cin >> i_input;
                cin.ignore(); // Oczyszczenie bufora po wprowadzeniu drugiej opcji
                switch (i_input) {
                    case 1: {
                        string file_name;
                        cout << "Podaj plik z instancja: ";
                        cin >> file_name;
                        read_from_file(file_name);
                        connectGraph();
                    } break;
                    case 2: {
                        string u_choice;
                        cout << "\nGenerowanie reczne\n";
                        cout << "Instancja standardowa? (T/N): ";
                        getline(cin, u_choice);
                        if(u_choice != "T" && u_choice != "t") read_input();
                        else input = base_input;
                        graph = generateInstance();
                        cout << "Wygenerowane DNA:\n" << input.dna;
                        cout << "\nElementow spektrum: " << graph.size();
                        cout << "\nZapisac instancje do pliku? (T/N): ";
                        cin >> u_choice;
                        if(u_choice == "T" || u_choice == "t"){
                            ofstream inFile("instancja.txt", ofstream::trunc);
                            inFile << input.dna;
                            inFile << "\n>spektrum:\n" << input.n << "\t" << input.first_oli;
                            for(unordered_map<string, Vertex>::iterator it = graph.begin(); it != graph.end(); it++){
                                inFile << "\n" << it->first;
                            }
                            inFile.close();
                        }
                        connectGraph();
                    } break;
                    default:
                        cout << "Nieprawidlowa opcja." << endl;
                    break;
                } break;
            }
            case 2:
                cout << "Algorytm Naiwny" << endl;
                // Kod dla algorytmu naiwnego
                break;

            case 3:
                cout << "Metaheurystyka" << endl;
                // Kod dla metaheurystyki
                break;

            case 4: {
                cout << "Milego dnia! Smacznej kawusi!\n\n";
                exit(0);
                }
            default:
                cout << "Nieprawidlowa opcja" << endl;
                break;
        }
    }
}

int main() {
    srand(time(nullptr));
    Menu();
}

//RAND() wymienic