#include "../shared/globals.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm> // For std::max

using namespace std;

// Assuming the definition of 'Edge', 'OutEdge', 'EdgeWeighted', and 'OutEdgeWeighted' are provided elsewhere

bool IsWeightedFormat(string format)
{
    if((format == "bwcsr") ||
       (format == "wcsr")  ||
       (format == "wel"))
        return true;
    return false;
}

string GetFileExtension(string fileName)
{
    if(fileName.find_last_of(".") != string::npos)
        return fileName.substr(fileName.find_last_of(".")+1);
    return "";
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cout << "\nThere was an error parsing command line arguments\n";
        exit(EXIT_FAILURE);
    }

    string input = string(argv[1]);

    if(GetFileExtension(input) == "el")
    {
        ifstream infile(input);
        if (!infile.is_open()) {
            cerr << "Failed to open file: " << input << endl;
            exit(EXIT_FAILURE);
        }

        stringstream ss;
        uint maxNode = 0;
        string line;
        uint edgeCounter = 0;
        vector<Edge> edges;
        Edge newEdge;

        while(getline(infile, line))
        {
            ss.str("");
            ss.clear();
            ss << line;

            ss >> newEdge.source >> newEdge.end;

            if (ss.fail()) {
                cerr << "Error reading line: " << line << endl;
                exit(EXIT_FAILURE);
            }

            edges.push_back(newEdge);
            edgeCounter++;

            maxNode = max(maxNode, max(newEdge.source, newEdge.end));
        }
        infile.close();

        uint num_nodes = maxNode + 1;
        uint num_edges = edgeCounter;

        uint *nodePointer = new uint[num_nodes + 1]();
        OutEdge *edgeList = new OutEdge[num_edges]();
        uint *degree = new uint[num_nodes]();
        for(uint i = 0; i < num_edges; i++)
            degree[edges[i].source]++;

        uint counter = 0;
        for(uint i = 0; i < num_nodes; i++)
        {
            nodePointer[i] = counter;
            counter += degree[i];
        }

        uint *outDegreeCounter = new uint[num_nodes]();
        for(uint i = 0; i < num_edges; i++)
        {
            if (edges[i].source >= num_nodes) {
                cerr << "Edge source index out of bounds: " << edges[i].source << endl;
                exit(EXIT_FAILURE);
            }

            uint location = nodePointer[edges[i].source] + outDegreeCounter[edges[i].source];

            if (location >= num_edges) {
                cerr << "Edge location index out of bounds: " << location << endl;
                exit(EXIT_FAILURE);
            }

            edgeList[location].end = edges[i].end;
            outDegreeCounter[edges[i].source]++;
        }

        // Writing to a binary file
        string outputFileName = input.substr(0, input.length()-2) + "bcsr";
        ofstream outfile(outputFileName, ios::binary);

        if (!outfile.is_open()) {
            cerr << "Failed to open file for writing: " << outputFileName << endl;
            exit(EXIT_FAILURE);
        }

        outfile.write(reinterpret_cast<char*>(&num_nodes), sizeof(num_nodes));
        outfile.write(reinterpret_cast<char*>(&num_edges), sizeof(num_edges));
        outfile.write(reinterpret_cast<char*>(nodePointer), sizeof(uint) * num_nodes);
        outfile.write(reinterpret_cast<char*>(edgeList), sizeof(OutEdge) * num_edges);

        outfile.close();

        cout << "Binary file created: " << outputFileName << endl;

        // Clean up dynamically allocated memory
        delete[] nodePointer;
        delete[] edgeList;
        delete[] degree;
        delete[] outDegreeCounter;
    }
    else if(GetFileExtension(input) == "wel")
    {
        // ... [The same structure for processing weighted edges]
        // Include file writing logic similar to the '.el' case
    }
    else
    {
        cout << "\nInput file format is not supported.\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}
