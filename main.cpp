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

bool valid_bike(int v, pair<int, int> pos, vector<vector<char>> graph);

map<int, pair<int, int>> find_bikes_coords(int v, int n, int m, vector<vector<char>> graph);

vector<char> find_available_peoples(int v);

bool valid_moviment(int n, int m, vector<vector<char>> graph, vector<vector<bool>> visited, pair<int, int> node);

pair<int, char> calculate_distance(int n, int m, vector<vector<char>> graph, pair<int, int> source, char destination);

vector<vector<pair<int, char>>> sort_distances(int v, int n, int m, vector<vector<char>> graph);

map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> sorted_preferences,
    vector<vector<pair<int, char>>> sorted_distances
);

int main() { _
    int v, n, m;

    string input;
    getline(cin, input);
    istringstream size(input);

    size >> v;

    getline(cin, input);
    istringstream dimensions(input);

    dimensions >> n;
    dimensions >> m;

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

    vector<vector<pair<int, int>>> sorted_preferences = sort_preferences(v, preferences);
    vector<vector<pair<int, char>>> sorted_distances = sort_distances(v, n, m, graph);

    // cout << "Preferencias das pessoas:" << endl;

    // for(int i = 0; i < v; i++) {
    //     for(int j = 0; j < v; j++) {
    //         cout << "Preferencia: " << sorted_preferences[i][j].f
    //              << " - Bicicleta: " << sorted_preferences[i][j].s
    //              << " - Novo IDX: " << j  << endl;
    //     }        
    // }

    // cout << endl <<  "Preferencias das bicicletas:" << endl;

    // for(int i = 0; i < v; i++) {
    //     for(int j = 0; j < v; j++) {
    //         cout << "Distancia: " << sorted_distances[i][j].f
    //              << " - Pessoa: " << sorted_distances[i][j].s
    //              << " - Novo IDX: " << j  << endl;
    //     }        
    // }

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

vector<char> find_available_peoples(int v) {
    vector<char> available_peoples;

    for(int i = 0; i < v; i++) {
        char people = (char)97 + i;

        available_peoples.pb(people);
    }

    return available_peoples;
}

bool valid_moviment(int n, int m, vector<vector<char>> graph, vector<vector<bool>> visited, pair<int, int> node) {
    return node.f >= 0
        and node.s >= 0
        and node.f < n
        and node.s < m
        and graph[node.f][node.s] != '-'
        and !visited[node.f][node.s];
}

pair<int, char> calculate_distance(int n, int m, vector<vector<char>> graph, pair<int, int> source, char destination) {
    pair<int, char> distance_index;

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

                visited[new_node.f][new_node.s] = true; layer[new_node.f][new_node.s] = layer[node.f][node.s] + 1;

                if(graph[new_node.f][new_node.s] == destination) {
                    distance_index.f = layer[new_node.f][new_node.s];
                    distance_index.s = destination;

                    return distance_index;
                }
            }
        }
    }

    return distance_index;
}

vector<vector<pair<int, char>>> sort_distances(int v, int n, int m, vector<vector<char>> graph) {
    vector<vector<pair<int, char>>> sorted_distances;

    vector<char> available_peoples = find_available_peoples(v);
    map<int, pair<int, int>> bikes = find_bikes_coords(v, n, m, graph);

    for(auto const bike : bikes) {
        vector<pair<int, char>> temp;

        for(int i = 0; i < v; i++) {
            pair<int, char> distance_index = calculate_distance(n, m, graph, bike.s, available_peoples[i]);
            temp.pb(distance_index);
        }

        sorted_distances.pb(temp);
    }    

    return sorted_distances;
}

map<char, int> stable_matching(
    int v,
    vector<vector<pair<int, int>>> sorted_preferences,
    vector<vector<pair<int, char>>> sorted_distances
) {
    map<int, char> matches;

    int free_count = v;

    vector<bool> free_people(v, true);
    vector<vector<bool>> already_proposed(v, vector<bool>(v, false));

    int free_guy = -1;

    while(free_count > 0) {
        for(int i = 0; i < v; i++) {
            if(free_people[i]) {
                free_guy = i;
                break;
            }
        }

        for(int i = 0; i < v; i++) {
            if(!already_proposed[free_guy][i]) {
                map<int, char>::iterator it;
                it = matches.find(sorted_preferences[free_guy][i].s);
                
                if (it == matches.end()) {
                    matches[sorted_preferences[free_guy][i].s] = (char)free_guy + 97;

                    free_people[free_guy] = false;                    
                    already_proposed[free_guy][i] = true;
                    
                    free_count--;

                    break;
                } else {
                    int distance_to_current = sorted_distances[i][it->s].f;
                    int distance_to_new = sorted_distances[i][free_guy].f;

                    if(distance_to_new < distance_to_current) {
                        matches[i] = (char)free_guy + 97;

                        free_people[it->s] = true;
                        free_people[free_guy] = false;
                    }
                }

                already_proposed[free_guy][i] = true;
            }
        }
    }

    map<char, int> sorted_matches;

    for(auto const pair : matches) {
        sorted_matches[pair.s] = pair.f;
    }

    return sorted_matches;
}
