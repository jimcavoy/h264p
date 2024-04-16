#ifndef NALU_H
#define NALU_H

#include <vector>
#include <memory>

#include <loki/Visitor.h>

namespace ThetaStream
{
    /// @brief Network Abstraction Layer (NAL) Unit.
    /// 
    class NALUnit
        : public Loki::BaseVisitable<>
    {
    public:
        typedef std::vector<uint8_t> RawByteStreamPayload;
        typedef RawByteStreamPayload::iterator iterator;
        typedef RawByteStreamPayload::const_iterator const_iterator;
    public:
        NALUnit(unsigned short startcodeprefix_len = 4);
        ~NALUnit();

        NALUnit& operator=(const NALUnit& rhs);
        NALUnit(const NALUnit& orig);

        NALUnit& operator=(NALUnit&& rhs) noexcept;
        NALUnit(NALUnit&& other) noexcept;

        unsigned short startcodeprefix_len() const;
        uint8_t nal_ref_idc() const;

        unsigned int size() const;
        void push_back(uint8_t c);
        void parse();

        iterator begin();
        iterator end();

        const uint8_t* data() const;

        void Accept(Loki::BaseVisitor& visitor);

    private:
        class Impl;
        std::unique_ptr<Impl> _pimpl;
    };

}

#endif