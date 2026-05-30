#pragma once

#include <dna/DNASequence.h>
#include <vector>

namespace dna {

enum class MutationType {
    Substitution,  
    Insertion,     
    Deletion       
};

struct MutationEvent {
    MutationType type;
    int index;
    Nucleotide oldBase;
    Nucleotide newBase;
};

class MutationEngine {
public:
    explicit MutationEngine(DNASequence& sequence);

    MutationEvent substitute(int index, Nucleotide newBase);
    MutationEvent insert(int index, Nucleotide base);
    MutationEvent deleteBase(int index);

    const std::vector<MutationEvent>& history() const { return m_history; }
    void clearHistory() { m_history.clear(); }

    const MutationEvent* lastMutation() const;

private:
    DNASequence& m_sequence;
    std::vector<MutationEvent> m_history;
};

} // namespace dna