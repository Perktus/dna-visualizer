#include <dna/DNASequence.h>
#include <stdexcept>
#include <random>

namespace dna {

// ── Nucleotide Color ── //
glm::vec3 nucleotideColor(Nucleotide n) {
    switch (n) {
        case Nucleotide::A: return { 0.95f, 0.26f, 0.21f }; // red
        case Nucleotide::T: return { 0.13f, 0.59f, 0.95f }; // blue
        case Nucleotide::G: return { 0.30f, 0.69f, 0.31f }; // green
        case Nucleotide::C: return { 1.00f, 0.76f, 0.03f }; // yellow
    }
    return { 1.0f, 1.0f, 1.0f }; // white
}

// ── Complement ── //
Nucleotide complement(Nucleotide n) {
    switch (n) {
        case Nucleotide::A: return Nucleotide::T;
        case Nucleotide::T: return Nucleotide::A;
        case Nucleotide::G: return Nucleotide::C;
        case Nucleotide::C: return Nucleotide::G;
    }
    return Nucleotide::A;
}

// ── From Char ── //
Nucleotide fromChar(char c) {
    switch (c) {
        case 'A': case 'a': return Nucleotide::A;
        case 'T': case 't': return Nucleotide::T;
        case 'G': case 'g': return Nucleotide::G;
        case 'C': case 'c': return Nucleotide::C;
        default: throw std::invalid_argument(std::string("Invalid nucleotide: ") + c);
    }
}

// ── To Char ── //
char toChar(Nucleotide n) {
    switch (n) {
        case Nucleotide::A: return 'A';
        case Nucleotide::T: return 'T';
        case Nucleotide::G: return 'G';
        case Nucleotide::C: return 'C';
    }
    return '?';
}

// ── Constructor ── //
DNASequence::DNASequence(const std::string& sequence) {
    m_strand.reserve(sequence.size());
    for (char c : sequence)
        m_strand.push_back(fromChar(c));
}

// ── Random ── //
DNASequence DNASequence::random(int length) {
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution<int> dist(0, 3);

    std::string seq;
    seq.reserve(length);
    const char bases[] = { 'A', 'T', 'G', 'C' };
    for (int i = 0; i < length; ++i)
        seq += bases[dist(rng)];

    return DNASequence(seq);
}

// ── Complementary ── //
DNASequence DNASequence::complementary() const {
    std::string seq;
    seq.reserve(m_strand.size());
    for (auto n : m_strand)
        seq += toChar(complement(n));
    return DNASequence(seq);
}

// ── ToString ── //
std::string DNASequence::toString() const {
    std::string result;
    result.reserve(m_strand.size());
    for (auto n : m_strand)
        result += toChar(n);
    return result;
}

// ── Mutation ── //
void DNASequence::mutate(int index, Nucleotide newType) {
    if (index < 0 || index >= static_cast<int>(m_strand.size()))
        throw std::out_of_range("Mutation index out of range");
    m_strand[index] = newType;
}

// ── Insertion ── //
void DNASequence::insert(int index, Nucleotide base) {
    if (index < 0 || index > static_cast<int>(m_strand.size()))
        throw std::out_of_range("Insert index out of range");
    m_strand.insert(m_strand.begin() + index, base);
}

// ── Deletion ── //
void DNASequence::remove(int index) {
    if (index < 0 || index >= static_cast<int>(m_strand.size()))
        throw std::out_of_range("Remove index out of range");
    m_strand.erase(m_strand.begin() + index);
}

} // namespace dna