#include <dna/MutationEngine.h>
#include <stdexcept>

namespace dna {

// ── Constructor ── //
MutationEngine::MutationEngine(DNASequence& sequence)
    : m_sequence(sequence)
{}

// ── Substitution ── //
MutationEvent MutationEngine::substitute(int index, Nucleotide newBase) {
    if (index < 0 || index >= m_sequence.length())
        throw std::out_of_range("Substitution index out of range");

    MutationEvent event{
        MutationType::Substitution,
        index,
        m_sequence.at(index),
        newBase
    };

    m_sequence.mutate(index, newBase);
    m_history.push_back(event);
    return event;
}

// ── Insertion ── //
MutationEvent MutationEngine::insert(int index, Nucleotide base) {
    if (index < 0 || index > m_sequence.length())
        throw std::out_of_range("Insertion index out of range");

    MutationEvent event{
        MutationType::Insertion,
        index,
        base,
        base
    };

    m_sequence.insert(index, base);
    m_history.push_back(event);
    return event;
}

// ── Deletion ── //
MutationEvent MutationEngine::deleteBase(int index) {
    if (index < 0 || index >= m_sequence.length())
        throw std::out_of_range("Deletion index out of range");

    MutationEvent event{
        MutationType::Deletion,
        index,
        m_sequence.at(index),
        m_sequence.at(index)
    };

    m_sequence.remove(index);
    m_history.push_back(event);
    return event;
}

// ── Last Mutation ── //
const MutationEvent* MutationEngine::lastMutation() const {
    if (m_history.empty()) return nullptr;
    return &m_history.back();
}

} // namespace dna