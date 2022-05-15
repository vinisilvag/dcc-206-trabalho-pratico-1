// 2021421869 - Vinicius Silva Gomes

#include <bits/stdc++.h>

using namespace std;

#define _ ios_base::sync_with_stdio(0); cin.tie(0);
#define endl '\n'

#define f first
#define s second
#define pb push_back

bool sort_comparison(pair<int, int>& pair1, pair<int, int>& pair2);

vector<vector<pair<int, int>>> sort_preferences(int v, vector<vector<int>> preferences);

bool valid_moviment(int n, int m, vector<vector<char>> graph, vector<vector<bool>> visited, pair<int, int> node);

bool is_visitor(char coord);

vector<int> bfs(int v, int n, int m, vector<vector<char>> graph, pair<int, int> source);

bool valid_bike(int v, pair<int, int> pos, vector<vector<char>> graph);

map<int, pair<int, int>> find_bikes_coords(int v, int n, int m, vector<vector<char>> graph);

vector<vector<int>> sort_distances(int v, int n, int m, vector<vector<char>> graph);

map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> sorted_preferences,
    vector<vector<int>> sorted_distances
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

    // Ordena as distâncias das bicicletas às pessoas ("matriz de preferência" das bicicletas)
    vector<vector<int>> sorted_distances = sort_distances(v, n, m, graph);

    // cout << "Preferencias das pessoas:" << endl;
    // for(int i = 0; i < v; i++) {
    //     cout << (char)(i + 97) << endl;
    //     for(int j = 0; j < v; j++) {
    //         cout << " Preferencia: " << sorted_preferences[i][j].f
    //              << " - Bicicleta: " << sorted_preferences[i][j].s
    //              << " - IDX: " << j  << endl;
    //     }        
    // }

    // cout << endl <<  "Preferencias das bicicletas:" << endl;
    // for(int i = 0; i < v; i++) {
    //     cout << i << endl;
    //     for(int j = 0; j < v; j++) {
    //         cout << " Distancia: " << sorted_distances[i][j]
    //              << " - Pessoa: " << (char)(j + 97) << endl;
    //     }        
    // }

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

bool sort_comparison(pair<int, int>& pair1, pair<int, int>& pair2) {
    return pair1.f > pair2.f;
}

vector<vector<pair<int, int>>> sort_preferences(int v, vector<vector<int>> preferences) {
    vector<vector<pair<int, int>>> sorted_preferences;

    for(int i = 0; i < v; i++) {
        vector<pair<int, int>> temp;

        for(int j = 0; j < v; j++) {
            temp.pb(pair<int, int>(preferences[i][j], j));
        }

        sort(temp.begin(), temp.end(), sort_comparison);

        sorted_preferences.pb(temp);
    }

    return sorted_preferences;
}

bool valid_moviment(int n, int m, vector<vector<char>> graph, vector<vector<bool>> visited, pair<int, int> node) {
    return node.f >= 0
        and node.s >= 0
        and node.f < n
        and node.s < m
        and graph[node.f][node.s] != '-'
        and !visited[node.f][node.s];
}

bool is_visitor(char coord) {
    int parsed_coord = (int)(coord - 97);

    return (parsed_coord >= 0 && parsed_coord <= 9);
}

vector<int> bfs(int v, int n, int m, vector<vector<char>> graph, pair<int, int> source) {
    vector<int> distances(v);

    vector<pair<int, int>> moviment = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    queue<pair<int, int>> queue;

    vector<vector<bool>> visited(n, vector<bool>(m, false));
    vector<vector<int>> layer(n, vector<int>(m, 0));

    queue.push(source); visited[source.f][source.s] = true; layer[source.f][source.s] = 0;

    while(!queue.empty()) {
        pair<int, int> node = queue.front(); queue.pop();

        for(auto new_node : moviment) {
            new_node.f += node.f; new_node.s += node.s;

            if(valid_moviment(n, m, graph, visited, new_node)) {
                queue.push(new_node);

                visited[new_node.f][new_node.s] = true;
                layer[new_node.f][new_node.s] = layer[node.f][node.s] + 1;

                if(is_visitor(graph[new_node.f][new_node.s])) {
                    int index = (int)(graph[new_node.f][new_node.s] - 97);
                    distances[index] = layer[new_node.f][new_node.s];
                }
            }
        }
    }

    return distances;
}

bool valid_bike(int v, pair<int, int> pos, vector<vector<char>> graph) {
    int pos_to_int = (int)graph[pos.f][pos.s] - 48;

    return pos_to_int >= 0 and pos_to_int < v;
}

map<int, pair<int, int>> find_bikes_coords(int v, int n, int m, vector<vector<char>> graph) {
    map<int, pair<int, int>> bikes;

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            pair<int, int> pos = {i, j};
            int char_to_int = (int)graph[i][j] - 48;

            if(valid_bike(v, pos, graph)) {
                bikes[char_to_int] = pos;
            }
        }
    }

    return bikes;
}

vector<vector<int>> sort_distances(int v, int n, int m, vector<vector<char>> graph) {
    vector<vector<int>> sorted_distances;

    map<int, pair<int, int>> bikes = find_bikes_coords(v, n, m, graph);

    for(auto const bike : bikes) {
        vector<int> distances = bfs(v, n, m, graph, bike.s);

        sorted_distances.pb(distances);
    }    

    return sorted_distances;
}

map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> sorted_preferences,
    vector<vector<int>> sorted_distances
) {
    map<int, char> matches;

    queue<int> free_visitors;
    vector<vector<bool>> already_proposed(v, vector<bool>(v, false));

    for(int i = 0; i < v; i++) {
        free_visitors.push(i);
    }

    while(!free_visitors.empty()) {
        int visitor = free_visitors.front(); free_visitors.pop();
        int bike = -1;

        for(int i = 0; i < v; i++) {
            if(!already_proposed[visitor][i]) {
                bike = i;
                break;
            }
        }

        // cout << (char)(visitor + 97) << " propondo para " << sorted_preferences[visitor][bike].s << "[" << bike << "]" << endl;

        map<int, char>::iterator it;
        it = matches.find(sorted_preferences[visitor][bike].s);

        if(it == matches.end()) {
            matches[sorted_preferences[visitor][bike].s] = (char)(visitor + 97);
        } else {
            int pair_index = (int)it->s - 97;

            int visitor_distance = sorted_distances[sorted_preferences[visitor][bike].s][visitor];
            int pair_distance = sorted_distances[sorted_preferences[visitor][bike].s][pair_index];
            
            // cout << "visitor: " << (char)(visitor + 97) << " pair: " << it->s << endl;
            // cout << "d_vis: " << visitor_distance << " d_pair: " << pair_distance << endl;

            if(visitor_distance < pair_distance) {
                matches[sorted_preferences[visitor][bike].s] = (char)(visitor + 97);
                free_visitors.push(pair_index);
            } else if(visitor_distance == pair_distance) {

                if(visitor < pair_index) {
                    matches[sorted_preferences[visitor][bike].s] = (char)(visitor + 97);
                    free_visitors.push(pair_index);
                } else {
                    free_visitors.push(visitor);
                }

            } else {
                free_visitors.push(visitor);
            }
        }

        already_proposed[visitor][bike] = true;
    }

    map<char, int> sorted_matches;

    for(auto const pair : matches) {
        sorted_matches[pair.s] = pair.f;
    }

    return sorted_matches;
}
