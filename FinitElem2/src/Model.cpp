#include "stdafx.h"
#include "Model.h"

Model::Model()
    : E_(1.0)
    , F_(1.0) {
}

bool Model::LoadFromFile(const std::string& fileName) {
    std::ifstream in(fileName, std::ios::in);
    
    if (!in) {
        return false;
    }
    
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("NODES ") == 0) {
            int node_count;
            sscanf(line.c_str(), "NODES %d", &node_count);
            nodes_.reserve(node_count);
            for (int i = 0; i < node_count; ++i) {
                std::getline(in, line);
                
                NODE n;
                sscanf(line.c_str(), "%d %lf %lf", &n.node, &n.x, &n.y);
                nodes_.push_back(n);
            }
            
        } else if (line.find("FIXED ") == 0) {
            int fixes_count;
            sscanf(line.c_str(), "FIXED %d", &fixes_count);
            fixes_.reserve(fixes_count);
            for (int i = 0; i < fixes_count; ++i) {
                std::getline(in, line);
                
                FIX f;
                sscanf(line.c_str(), "%d %d", &f.node, &f.axis);
                fixes_.push_back(f);
            }
            
        } else if (line.find("LOADS ") == 0) {
            int load_count;
            sscanf(line.c_str(), "LOADS %d", &load_count);
            loads_.reserve(load_count);
            for (int i = 0; i < load_count; ++i) {
                std::getline(in, line);
                
                LOAD l;
                sscanf(line.c_str(), "%d %lf %lf", &l.node, &l.px, &l.py);
                loads_.push_back(l);
            }
            
        } else if (line.find("ELEMENTS ") == 0) {
            int element_count;
            sscanf(line.c_str(), "ELEMENTS %d", &element_count);
            elements_.reserve(element_count);
            for (int i=0; i<element_count; i++) {
                std::getline(in, line);
                
                ELEM e;
                sscanf(line.c_str(), "%d %d", &e.nodes[0], &e.nodes[1]);
                
                // Calculate element parameters
                NODE n1 = nodes_[e.nodes[0]-1];
                NODE n2 = nodes_[e.nodes[1]-1];
                double dx = n2.x - n1.x;
                double dy = n2.y - n1.y;
                
                e.length = sqrt(dx * dx + dy * dy);
                e.cosa = dx / e.length;
                e.sina = dy / e.length;
                
                elements_.push_back(e);
            }
            
        } else if (line.find("E ") == 0) {
            sscanf(line.c_str(), "E %lf", &E_);
            
        } else if (line.find("F ") == 0) {
            sscanf(line.c_str(), "F %lf", &F_);
        }
    }
    
    in.close();
    
    return true;
}

void Model::PrepareSolve() {
    component_count_ = nodes_.size() * g_modelDimensions;
    
    b_ = arma::vec(component_count_, arma::fill::zeros);
    u_ = arma::vec(component_count_, arma::fill::zeros);
    n_ = arma::vec(elements_.size(), arma::fill::zeros);
    
    a_ = arma::mat(component_count_, component_count_, arma::fill::zeros);
    
    // Fill B matrix
    for (const auto& load : loads_) {
        b_(load.node * 2 - 2) = load.px;
        b_(load.node * 2 - 1) = load.py;
    }
    
    // Fill A matrix
    for (const auto& elem : elements_) {
        NODE n1 = nodes_[elem.nodes[0] - 1];
        NODE n2 = nodes_[elem.nodes[1] - 1];
        
        double sina = elem.sina;
        double cosa = elem.cosa;
        double length = elem.length;
        
        int globalIndices[4];
        globalIndices[0] = n1.node * 2 - 1;
        globalIndices[1] = n1.node * 2;
        globalIndices[2] = n2.node * 2 - 1;
        globalIndices[3] = n2.node * 2;
        
        // Fill K matrix for each element
        float K[4][4];
        K[0][0] = cosa * cosa / length;
        K[0][1] = K[1][0] = cosa * sina / length;
        K[1][1] = sina * sina / length;
        
        for (int j=0; j<2; j++) {
            for (int m=0; m<2; m++) {
                K[j+2][m+2] = K[j][m];
                K[j+2][m] = K[j][m+2] = -K[j][m];
            }
        }
        
        // Copy values from K to A matrix according to global coords
        for (int j=0; j<4; j++) {
            for (int m=0; m<4; m++) {
                int p = globalIndices[j] - 1;
                int q = globalIndices[m] - 1;
                a_(p, q) += K[j][m];
            }
        }
    }
    
    // Convert fixes
    for (const auto& fix : fixes_) {
        int node = (fix.node - 1) * 2 + 1 - (fix.axis % 2);
        for (int i = 0; i < component_count_; ++i) {
            a_(node, i) = 0.0;
            a_(i, node) = 0.0;
        }
        a_(node, node) = 1.0;
        b_(node) = 0.0;
    }
}

void Model::Solve() {
    // Solve system of linear equations A*u=b
    u_ = a_.i() * b_;
    
    u_ /= (E_ * F_);
    
    size_t idx = 0;
    for (const auto& elem : elements_) {
        NODE n1 = nodes_[elem.nodes[0] - 1];
        NODE n2 = nodes_[elem.nodes[1] - 1];
        double nx = (u_(2*n1.node - 2) - u_(2*n2.node - 2)*(n1.x - n2.x));
        double ny = (u_(2*n1.node - 1) - u_(2*n2.node - 1)*(n1.y - n2.y));
        n_(idx++) = (nx + ny) / (elem.length * elem.length);
    }
}
