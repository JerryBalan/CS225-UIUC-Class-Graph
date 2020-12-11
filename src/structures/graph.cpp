#include "graph.h"
#include "edge.h"
#include <map>
#include <queue>
#include <vector>

const Vertex Graph::InvalidVertex = "_CS225INVALIDVERTEX";

const string Graph:: InvalidLabel = "_CS225INVALIDLABEL";
const Edge Graph::InvalidEdge = Edge(Graph::InvalidVertex, Graph::InvalidVertex, Graph::InvalidLabel);

Graph::Graph() : directed(true)
{
}

//imported from CS225 class
vector<Vertex> Graph::getAdjacent(Vertex source) const //function to help find direct prerequesites of a class
{
    auto lookup = adjacency_list.find(source);

    if(lookup == adjacency_list.end())
        return vector<Vertex>();

    else
    {
        vector<Vertex> vertex_list;
        unordered_map <Vertex, Edge> & map = adjacency_list[source];
        for (auto it = map.begin(); it != map.end(); it++)
        {
            vertex_list.push_back(it->first);
        }
        return vertex_list;
    }
}

//imported from CS225 class
Vertex Graph::getStartingVertex() const 
{
    return adjacency_list.begin()->first;
}

//imported from CS225 class
vector<Vertex> Graph::getVertices() const
{
    vector<Vertex> ret;

    for(auto it = adjacency_list.begin(); it != adjacency_list.end(); it++)
    {
        ret.push_back(it->first);
    }

    return ret;
}

//imported from CS225 class
Edge Graph::getEdge(Vertex source , Vertex destination) const
{
    if(assertEdgeExists(source, destination, __func__) == false)
        return Edge();
    Edge ret = adjacency_list[source][destination];
    return ret;
}

//imported from CS225 class
vector<Edge> Graph::getEdges() const
{
    if (adjacency_list.empty())
        return vector<Edge>();

    vector<Edge> ret;
    set<pair<Vertex, Vertex>> seen;

    for (auto it = adjacency_list.begin(); it != adjacency_list.end(); it++)
    {
        Vertex source = it->first;
        for (auto its = adjacency_list[source].begin(); its != adjacency_list[source].end(); its++)
        {
            Vertex destination = its->first;
            if(seen.find(make_pair(source, destination)) == seen.end())
            {
                //this pair is never added to seen
                ret.push_back(its->second);
                seen.insert(make_pair(source,destination));
                if(!directed)
                {
                    seen.insert(make_pair(destination, source));
                }
            }
        }
    }

    return ret;
}

//imported from CS225 class
bool Graph::vertexExists(Vertex v) const
{
    return assertVertexExists(v, "");
}

//imported from CS225 class
bool Graph::edgeExists(Vertex source, Vertex destination) const
{
    return assertEdgeExists(source, destination, "");
}

//imported from CS225 class
Edge Graph::setEdgeLabel(Vertex source, Vertex destination, string label)
{
    if (assertEdgeExists(source, destination, __func__) == false)
        return InvalidEdge;
    Edge e = adjacency_list[source][destination];
    Edge new_edge(source, destination, label);
    adjacency_list[source][destination] = new_edge;

    if(!directed)
    {
        Edge new_edge_reverse(destination,source, label);
        adjacency_list[destination][source] = new_edge_reverse;
    }
    return new_edge;
}

//imported from CS225 class
string Graph::getEdgeLabel(Vertex source, Vertex destination) const
{
    if(assertEdgeExists(source, destination, __func__) == false)
        return InvalidLabel;
    return adjacency_list[source][destination].getLabel();
}

//imported from CS225 class
void Graph::insertVertex(Vertex v)
{
    // will overwrite if old stuff was there
    removeVertex(v);
    // make it empty again
    adjacency_list[v] = unordered_map<Vertex, Edge>();
}

//imported from CS225 class
Vertex Graph::removeVertex(Vertex v)
{

    if (adjacency_list.find(v) != adjacency_list.end())
    {
        if(!directed){
            for (auto it = adjacency_list[v].begin(); it != adjacency_list[v].end(); it++)
            {
                Vertex u = it->first;
                adjacency_list[u].erase(v); 
            }
            adjacency_list.erase(v);
            return v;
        }
        
        adjacency_list.erase(v);
        for(auto it2 = adjacency_list.begin(); it2 != adjacency_list.end(); it2++)
        {
            Vertex u = it2->first;
            if (it2->second.find(v)!=it2->second.end())
            {
                it2->second.erase(v);
            }
        }
        return v;
    }

    return InvalidVertex;
}

//imported from CS225 class
bool Graph::insertEdge(Vertex source, Vertex destination)
{
    if(adjacency_list.find(source)!= adjacency_list.end() 
    && adjacency_list[source].find(destination)!= adjacency_list[source].end())
    {
        //edge already exit
        return false;
    }

    if(adjacency_list.find(source)==adjacency_list.end())
    {
        adjacency_list[source] = unordered_map<Vertex, Edge>();
    }
        //source vertex exists
    adjacency_list[source][destination] = Edge(source, destination);
    if(!directed)
    {
        if(adjacency_list.find(destination)== adjacency_list.end())
        {
            adjacency_list[destination] = unordered_map<Vertex, Edge>();
        }
        adjacency_list[destination][source] = Edge(source, destination);
    }
    
    return true;
}

//imported from CS225 class
Edge Graph::removeEdge(Vertex source, Vertex destination)
{
    if(assertEdgeExists(source, destination, __func__) == false)
        return InvalidEdge;
    Edge e = adjacency_list[source][destination];
    adjacency_list[source].erase(destination);
    // if undirected, remove the corresponding edge
    if(!directed)
    {
        adjacency_list[destination].erase(source);
    }
    return e;
}

//imported from CS225 class
bool Graph::assertVertexExists(Vertex v, string functionName) const
{
    if (adjacency_list.find(v) == adjacency_list.end())
    {
        if (functionName != "")
            error(functionName + " called on nonexistent vertices");
        return false;
    }
    return true;
}

//imported from CS225 class
bool Graph::assertEdgeExists(Vertex source, Vertex destination, string functionName) const
{
    if(assertVertexExists(source,functionName) == false)
        return false;
    if(adjacency_list[source].find(destination)== adjacency_list[source].end())
    {
        if (functionName != "")
            error(functionName + " called on nonexistent edge " + source + " -> " + destination);
        return false;
    }

    if(!directed)
    {
        if (assertVertexExists(destination,functionName) == false)
            return false;
        if(adjacency_list[destination].find(source)== adjacency_list[destination].end())
        {
            if (functionName != "")
                error(functionName + " called on nonexistent edge " + destination + " -> " + source);
            return false;
        }
    }
    return true;
}

//imported from CS225 class
bool Graph::isDirected() const
{
    return directed;
}

//imported from CS225 class
void Graph::clear()
{
    adjacency_list.clear();
}

void Graph::BFS(Vertex startVertex, vector <Vertex> &output) { //output is our vector containing the BFS traversal
    output.clear();
    std::map<Vertex, bool> visited;
    std::queue<Vertex> queue;

    visited.insert({startVertex, true});
    queue.push(startVertex);
    std::vector<Vertex> adjacents;
    Vertex currVertex;

    while (!queue.empty()) { //loop while all prerequesites haven't been visited as currVertex
        currVertex = queue.front();
        queue.pop();
        output.push_back(currVertex);
        adjacents = getAdjacent(currVertex); //find vertices that are prerequistes of the currVertex class

        for (auto i : adjacents) {
            if (visited.find(i) == visited.end()) { //if the adjacent class isn't in the map yet (not been visited as currVertex)
                visited.insert({i, true}); //put into map
                queue.push(i); //add to queue
            }
        }
    }
}

//imported from CS225 class
/**
 * Prints a graph error and quits the program.
 * The program is exited with a segfault to provide a stack trace.
 * @param message - the error message that is printed
 */
void Graph::error(string message) const
{
    cerr << "\033[1;31m[Graph Error]\033[0m " + message << endl;
}

//imported from CS225 class
/**
 * Creates a name for snapshots of the graph.
 * @param title - the name to save the snapshots as
 */
void Graph::initSnapshot(string title)
{
    picNum = 0;
    picName = title;
}

//imported from CS225 class
/**
 * Saves a snapshot of the graph to file.
 * initSnapshot() must be run first.
 */
void Graph::snapshot()
{
    std::stringstream ss;
    ss << picNum;
    string newName = picName + ss.str();
    savePNG(newName);
    ++picNum;
}

//imported from CS225 class
/**
 * Prints the graph to stdout.
 */
void Graph::print() const
{
    for (auto it = adjacency_list.begin(); it != adjacency_list.end(); ++it) 
    {
        cout << it->first << endl;
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) 
        {
            std::stringstream ss;
            ss << it2->first; 
            string vertexColumn = "    => " + ss.str();
            vertexColumn += " " ;
            cout << std::left << std::setw(26) << vertexColumn;
            string edgeColumn = "edge label = \"" + it2->second.getLabel()+ "\"";
            cout << std::left << std::setw(26) << edgeColumn;
            cout << endl;
        }
        cout << endl;
    }
}

//imported from CS225 class but edited by us to make it a force directed graph
/**
 * Saves the graph as a PNG image.
 * @param title - the filename of the PNG image
 */
void Graph::savePNG(string title) const
{
    std::ofstream neatoFile;
    string filename = "images/" + title + ".dot";
    neatoFile.open(filename.c_str());

    if (!neatoFile.good())
        error("couldn't create " + filename + ".dot");

    neatoFile
        << "strict graph G {\n"
        << "\toverlap=\"false\";\n"
        << "\tdpi=\"1300\";\n"
        << "\tsep=\"1.5\";\n"
        << "\tnode [fixedsize=\"true\", shape=\"circle\", fontsize=\"7.0\"];\n"
        << "\tedge [penwidth=\"1.5\", fontsize=\"7.0\"];\n";

    vector<Vertex> allv = getVertices();
    //lambda expression
    sort(allv.begin(), allv.end(), [](const Vertex& lhs, const Vertex& rhs) {
        return stoi(lhs.substr(3)) > stoi(rhs.substr(3));
    });

    int xpos1 = 100;
    int xpos2 = 100;
    int xpos, ypos;
    for (auto it : allv) {
        string current = it;
        neatoFile 
            << "\t\"" 
            << current
            << "\"";
        if (current[1] == '1') {
            ypos = 100;
            xpos = xpos1;
            xpos1 += 100;
        }
        else {
            ypos = 200;
            xpos = xpos2;
            xpos2 += 100;
        }
        neatoFile << "[pos=\""<< xpos << "," << ypos <<"\"]";
        neatoFile << ";\n";
    }

    neatoFile << "\tedge [penwidth=\"1.5\", fontsize=\"7.0\"];\n";

    for (auto it = adjacency_list.begin(); it != adjacency_list.end(); ++it) 
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) 
        {
            string vertex1Text = it->first;
            string vertex2Text = it2->first;

            neatoFile << "\t\"" ;
            neatoFile << vertex1Text;
            neatoFile << "\" -- \"" ;
            neatoFile << vertex2Text;
            neatoFile << "\"";

            string edgeLabel = it2->second.getLabel();
            if (edgeLabel == "WIN") {
                neatoFile << "[color=\"blue\"]";
            } else if (edgeLabel == "LOSE") {
                neatoFile << "[color=\"red\"]";                
            } else {
                neatoFile << "[color=\"grey\"]";
            }
            
            
            neatoFile<< "[constraint = \"false\"]" << ";\n";
        }
    }

    neatoFile << "}";
    neatoFile.close();
    string command = "neato -n -Tpng " + filename + " -o " + "images/" + title
                     + ".png 2> /dev/null";
    int result = system(command.c_str());


    if (result == 0) {
        cout << "Output graph saved as images/" << title << ".png" << endl;
    } else {
        cout << "Failed to generate visual output graph using `neato`. Install `graphviz` or `neato` to generate a visual graph." << endl;
    }

    string rmCommand = "rm -f " + filename + " 2> /dev/null";
    system(rmCommand.c_str());
}
