#include "stdafx.h"
#include "Model.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [model file]" << std::endl;
        return 0;
    }
    
    std::string modelFile(argv[1]);
    Model model;
    if (!model.LoadFromFile(modelFile)) {
        std::cerr << "Unable to load model from file " << modelFile << std::endl;
        return 0;
    }
    
    std::cout << "Loaded model from file: " << modelFile << std::endl;
    std::cout << "Nodes: " << model.nodes_.size() << std::endl;
    std::cout << "Fixes: " << model.fixes_.size() << std::endl;
    std::cout << "Loads: " << model.loads_.size() << std::endl;
    std::cout << "Elements: " << model.elements_.size() << std::endl;
    
    model.PrepareSolve();
    std::cout << "Prepared solution" << std::endl;
    std::cout << "Created A and B" << std::endl;
    std::cout << "Matrix A: " << std::endl << model.a_ << std::endl;
    std::cout << "vector B: " << std::endl << model.b_ << std::endl;
    
    model.Solve();
    std::cout << "Solved model" << std::endl;
    std::cout << "Vector U: " << std::endl << model.u_ << std::endl;
    std::cout << "vector N: " << std::endl << model.n_ << std::endl;
    
    return 0;
}
