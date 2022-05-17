// 2021421869 - Vinicius Silva Gomes

#include <bits/stdc++.h>

using namespace std;

#define _ ios_base::sync_with_stdio(0); cin.tie(0);
#define endl '\n'

#define f first
#define s second
#define pb push_back

// Redefine a função de comparação da função sort para ordernar pares em ordem decrescente
bool sort_comparison(pair<int, int>& pair1, pair<int, int>& pair2);

/*
    Ordena as preferências dos visitantes, criando uma matriz de pares que contém
    preferência do visitante em relação a bicicleta e o identificador daquela bicicleta
    (caso o identificador não fosse salvo, a referência ao identificador da bicicleta
    seria perdida, por isso ele é salvo como um atributo e, assim, a ordenação pode ser feita)
*/
vector<vector<pair<int, int>>> sort_preferences(int v, vector<vector<int>> &preferences);

/*
    Verifica se a célula passada como parâmetro pode ser navegável (não foi visitada, 
    não é um obstáculo, não supera os limites do mapa, etc)
*/
bool valid_moviment(int n, int m, vector<vector<char>> &graph, vector<vector<bool>> &visited, pair<int, int> cell);

/* Verifica se a célula passada como parâmetro representa um visitante */
bool is_visitor(char coord);

/*
    Calcula as distâncias entre a bicicleta passada como fonte (source) e os visitantes
    presentes no mapa. O retorno é um vetor onde o valor do index i representa a distância
    da bicicleta source até o visitante i.
*/
vector<int> bfs(int v, int n, int m, vector<vector<char>> &graph, pair<int, int> source);

// Verifica se a célula passada como parâmetro representa uma bicicleta
bool is_bike(char coord);

/*
    Percorre o mapa e identifica as células do mapa que contém bicicletas e salva essas
    células em um mapa de pares, para facilitar o cálculo das distâncias
*/ 
map<int, pair<int, int>> find_bikes_coords(int n, int m, vector<vector<char>> &graph);

/*
    Busca a posição de todas as bicicletas no mapa, percorre o mapa identificando as distâncias
    entre essas bicicletas e as pessoas no mapa e, por fim, concatena os vetores de distância em
    uma matriz, criando a tabela de preferências para as bicicletas
*/
vector<vector<int>> sort_distances(int v, int n, int m, vector<vector<char>> &graph);

/*
    Recebe instâncias das tabelas de preferência dos visitantes e das bicicletas e 
    executa o algoritmo de Gale-Shapley, retornando um emparelhamento estável que
    respeite os critérios estabelecidos pelo problema
*/
map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> &sorted_preferences,
    vector<vector<int>> &sorted_distances
);

int main() { _
    int v, n, m;

    // Recebe as entradas do arquivo de entrada
    string input;
    getline(cin, input);
    istringstream size(input);

    size >> v;

    getline(cin, input);
    istringstream dimensions(input);

    dimensions >> n;
    dimensions >> m;

    // Declara o mapa (grafo) dado pelas entradas e a matriz de preferências das pessoas
    vector<vector<char>> graph(n, vector<char>(m));
    vector<vector<int>> preferences(v, vector<int>(v));

    // Lê o mapa e a tabela de preferências não ordenada do arquivo
    for(int i = 0; i < n; i++) {
        getline(cin, input);
        istringstream map(input);

        for(int j = 0; j < m; j++) {
            map >> graph[i][j];
        }
    }

    for(int i = 0; i < v; i++) {
        getline(cin, input);
        istringstream preference(input);

        for(int j = 0; j < v; j++) {
            preference >> preferences[i][j];
        }
    }

    // Ordena as preferências das pessoas de maneira decrescente (matriz de preferência das pessoas)
    vector<vector<pair<int, int>>> sorted_preferences = sort_preferences(v, preferences);

    // Cria e ordena a tabela das distâncias das bicicletas às pessoas ("matriz de preferência" das bicicletas)
    vector<vector<int>> sorted_distances = sort_distances(v, n, m, graph);

    /*
        Calcula o Stable Matching a partir do algoritmo de Gale-Shapley
        e exibe os resultados no for em seguida.
    */
    map<char, int> matches = stable_matching(v, sorted_preferences, sorted_distances);

    for(auto const stable_pair : matches) {
        cout << stable_pair.f << " " << stable_pair.s << endl;
    }

    return 0;
}

bool sort_comparison(pair<int, int> &pair1, pair<int, int> &pair2) {
    return pair1.f > pair2.f;
}

vector<vector<pair<int, int>>> sort_preferences(int v, vector<vector<int>> &preferences) {
    vector<vector<pair<int, int>>> sorted_preferences;

    for(int i = 0; i < v; i++) {
        vector<pair<int, int>> temp;

        for(int j = 0; j < v; j++) {
            // Insere o "valor" da preferência e a posição original da bicicleta (identificador) no par
            temp.pb(pair<int, int>(preferences[i][j], j));
        }

        // Ordena o vetor de pares a partir do valor da preferência
        sort(temp.begin(), temp.end(), sort_comparison);

        // Insere o vetor ordenado na tabela
        sorted_preferences.pb(temp);
    }

    return sorted_preferences;
}

bool valid_moviment(int n, int m, vector<vector<char>> &graph, vector<vector<bool>> &visited, pair<int, int> cell) {
    // Faz o teste para avaliar se essa posição no grafo é navegável ou não
    return cell.f >= 0
        and cell.s >= 0
        and cell.f < n
        and cell.s < m
        and graph[cell.f][cell.s] != '-'
        and !visited[cell.f][cell.s];
}

bool is_visitor(char coord) {
    // Converte a coordenada para inteiro
    int parsed_coord = (int)(coord - 97);

    /*
        Como visitantes vão de "a" a "j", é possível converter para inteiro e subtrair 97 
        (valor do caractere "a" na tabela ASCII), obendo um novo range de 0 a 9, caso for
        um visitante
    */
    return parsed_coord >= 0 and parsed_coord <= 9;
}

vector<int> bfs(int v, int n, int m, vector<vector<char>> &graph, pair<int, int> source) {
    vector<int> distances(v);

    // Vetor de possíveis movimentos dentro do grafo
    vector<pair<int, int>> moviment = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Fila que armazena as posições que ainda não foram exploradas no grafo
    queue<pair<int, int>> q;

    // Matriz que armazena quais células da matriz já foram visitadas
    vector<vector<bool>> visited(n, vector<bool>(m, false));

    // Vetor com as camadas geradas pela BFS (distância mínima entre uma célula e a fonte)
    vector<vector<int>> layer(n, vector<int>(m, -1));

    q.push(source);
    visited[source.f][source.s] = true;
    layer[source.f][source.s] = 0;

    while(!q.empty()) {
        // Pega o primeiro elemento da fila e o remove
        pair<int, int> cell = q.front();
        q.pop();

        for(auto mov : moviment) {
            // "Anda" dentro do grafo através do vetor de movimentos
            pair<int, int> new_cell = {cell.f + mov.f, cell.s + mov.s};

            /* 
                Se o movimento for possível ele insere a célula na fila, marca o vetor de 
                visitados e calcula a camada da célula que foi visitada
            */
            if(valid_moviment(n, m, graph, visited, new_cell)) {
                q.push(new_cell);
                visited[new_cell.f][new_cell.s] = true;
                layer[new_cell.f][new_cell.s] = layer[cell.f][cell.s] + 1;

                /*
                    Se a célula válida for um visitante, insere a distância da bicicleta até esse 
                    visitante no vetor de distâncias
                */
                if(is_visitor(graph[new_cell.f][new_cell.s])) {
                    int index = (int)(graph[new_cell.f][new_cell.s] - 97);
                    distances[index] = layer[new_cell.f][new_cell.s];
                }
            }
        }
    }

    return distances;
}

bool is_bike(char coord) {
    // Converte a coordenada para inteiro
    int parsed_coord = (int)(coord - 48);

    /*
        Como visitantes vão de "0" a "9", é possível converter para inteiro e subtrair 48 
        (valor do caractere "0" na tabela ASCII), obendo um novo range de 0 a 9, caso for
        um visitante
    */
    return parsed_coord >= 0 and parsed_coord <= 9;
}

map<int, pair<int, int>> find_bikes_coords(int n, int m, vector<vector<char>> &graph) {
    map<int, pair<int, int>> bikes;

    // Percorre o mapa buscando por células com bicicletas
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(is_bike(graph[i][j])) {
                int index = (int)(graph[i][j] - 48);

                // Se for bicicleta, insere no mapa as coordenadas daquela bicicleta
                bikes[index] = pair<int, int>(i, j);
            }
        }
    }

    return bikes;
}

vector<vector<int>> sort_distances(int v, int n, int m, vector<vector<char>> &graph) {
    vector<vector<int>> sorted_distances;

    // Computa as coordenadas das bicicletas presentes no mapa
    map<int, pair<int, int>> bikes = find_bikes_coords(n, m, graph);

    for(auto const bike : bikes) {
        // Calcula o vetor com as distâncias 
        vector<int> distances = bfs(v, n, m, graph, bike.s);

        // Insere o vetor na tabela de preferências das bicicletas
        sorted_distances.pb(distances);
    }    

    return sorted_distances;
}

map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> &sorted_preferences,
    vector<vector<int>> &sorted_distances
) {
    map<int, char> matches;

    queue<int> free_visitors;
    // Inicializa a matriz de propostas de visitantes às bicicletas com false
    vector<vector<bool>> already_proposed(v, vector<bool>(v, false));

    // Inicializa a lista de visitantes livres com todos os visitantes presentes
    for(int i = 0; i < v; i++) {
        free_visitors.push(i);
    }

    // Enquanto existir algum visitante sem par
    while(!free_visitors.empty()) {
        // Seleciona um visitante da fila
        int visitor = free_visitors.front(); free_visitors.pop();
        int not_proposed_bike = -1;

        // Seleciona uma bicicleta que ele ainda não propôs
        for(int i = 0; i < v; i++) {
            if(!already_proposed[visitor][i]) {
                not_proposed_bike = i;
                break;
            }
        }

        // Busca o identificador dessa bicicleta na lista de preferências dele
        int bike_index = sorted_preferences[visitor][not_proposed_bike].s;

        map<int, char>::iterator it;
        it = matches.find(bike_index);

        // Bicicleta com esse identificador não possui par
        if(it == matches.end()) {
            // Forma um par com o visitante selecionado
            matches[bike_index] = (char)(visitor + 97);
        } else {
            int pair_index = (int)it->s - 97;

            int visitor_distance = sorted_distances[bike_index][visitor];
            int pair_distance = sorted_distances[bike_index][pair_index];

            // Caso não compara as distâncias entre o visitante e o atual par da bicicleta
            if(visitor_distance < pair_distance) {
                /*
                    Se a distância do visitante for menor ele forma um par com a bicicleta e 
                    o antigo par volta para a lista
                */ 
                matches[bike_index] = (char)(visitor + 97);
                free_visitors.push(pair_index);
            } else if(visitor_distance == pair_distance) {

                // Caso as distâncias sejam iguais, compara pelo identificador
                if(visitor < pair_index) {
                    // Se o ID do visitante for menor que o do atual par, ele vira o novo par da bicicleta
                    matches[bike_index] = (char)(visitor + 97);
                    free_visitors.push(pair_index);
                } else {
                    free_visitors.push(visitor);
                }

            } else {
                free_visitors.push(visitor);
            }
        }

        // Independente do que aconteceu, marca que aquele visitante propôs para a bicicleta selecionada
        already_proposed[visitor][not_proposed_bike] = true;
    }

    /*
        Para facilitar o acesso ao mapa de emparelhamentos, ele foi declarado como sendo do tipo <int, char>.
        Assim, é possível acessar diretamente uma bicicleta e verificar se ela tem um par. No entanto, como a
        resposta pedida pelo enunciada é ordenada pelo ID do visitante, esse trecho inverte a ordem do par, 
        aproveitando da ordenação natural do map 
    */
    map<char, int> sorted_matches;

    for(auto const pair : matches) {
        sorted_matches[pair.s] = pair.f;
    }

    return sorted_matches;
}
