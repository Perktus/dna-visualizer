#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace dna {

enum class Nucleotide { A, T, G, C };
glm::vec3 nucleotideColor(Nucleotide n);
Nucleotide complement(Nucleotide n);
Nucleotide fromChar(char c);
char toChar(Nucleotide n);

class DNASequence {
public:
    explicit DNASequence(const std::string& sequence);

    static DNASequence random(int length);

    const std::vector<Nucleotide>& strand() const { return m_strand; }
    int length() const { return static_cast<int>(m_strand.size()); }
    Nucleotide at(int i) const { return m_strand[i]; }

    DNASequence complementary() const;

    void mutate(int index, Nucleotide newType);

    std::string toString() const;

private:
    std::vector<Nucleotide> m_strand;
};

} // namespace dna