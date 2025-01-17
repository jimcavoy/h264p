#ifndef NALUIMPL_H
#define NALUIMPL_H

#include <vector>

#include "sei.h"

#include <loki/Visitor.h>

/// @file Contains class definitions for NAL Unit Implementation types.

namespace ThetaStream
{
    /// @brief Base NAL Unit Implementation class
    class NALUnitImpl
        : public Loki::BaseVisitable<>
    {
    public:
        typedef std::vector<uint8_t> RawByteStreamPayload;
        typedef RawByteStreamPayload::iterator iterator;
    public:
        NALUnitImpl(uint8_t nut);
        virtual ~NALUnitImpl();

        virtual void parse(NALUnitImpl::iterator first
            , NALUnitImpl::iterator last) = 0;

    protected:
        unsigned char nal_unit_type;
    };

    /// @brief Coded slice data partition A
    class NALUnitDPA
        : public NALUnitImpl
    {
    public:
        NALUnitDPA();
        virtual ~NALUnitDPA();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Coded slice data partition B
    class NALUnitDPB
        : public NALUnitImpl
    {
    public:
        NALUnitDPB();
        virtual ~NALUnitDPB();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief Coded slice data partition A
    class NALUnitDPC
        : public NALUnitImpl
    {
    public:
        NALUnitDPC();
        virtual ~NALUnitDPC();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief Supplemental Enhancement Information
    class NALUnitSEI
        : public NALUnitImpl
    {
    public:
        typedef std::vector<SEI> sei_payloads_type;
        typedef sei_payloads_type::iterator iterator;
    public:
        NALUnitSEI();
        virtual ~NALUnitSEI();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()

            sei_payloads_type::iterator begin();
        sei_payloads_type::iterator end();

    private:
        sei_payloads_type _sei_payloads;
    };

    /// @brief Access Unit Delimiter
    class NALUnitAUD
        : public NALUnitImpl
    {
    public:
        NALUnitAUD();
        virtual ~NALUnitAUD();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        unsigned short primary_pic_type() const;

        LOKI_DEFINE_VISITABLE()

    private:
        unsigned char primary_pic_type_;
    };

    /// @brief End of Sequence
    class NALUnitEOSEQ
        : public NALUnitImpl
    {
    public:
        NALUnitEOSEQ();
        virtual ~NALUnitEOSEQ();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief End of Stream
    class NALUnitEOStream
        : public NALUnitImpl
    {
    public:
        NALUnitEOStream();
        virtual ~NALUnitEOStream();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Filler Data
    class NALUnitFill
        : public NALUnitImpl
    {
    public:
        NALUnitFill();
        virtual ~NALUnitFill();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Sequence Parameter Set Extension
    class NALUnitSPSE
        : public NALUnitImpl
    {
    public:
        NALUnitSPSE();
        virtual ~NALUnitSPSE();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Prefix NAL Unit
    class NALUnitPrefixNALU
        : public NALUnitImpl
    {
    public:
        NALUnitPrefixNALU();
        virtual ~NALUnitPrefixNALU();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief Subset sequence parameter set
    class NALUnitSubsetSPS
        : public NALUnitImpl
    {
    public:
        NALUnitSubsetSPS();
        virtual ~NALUnitSubsetSPS();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief Coded slice of an auxiliary coded picture without partitioning
    class NALUnitSliceWithoutPartitioning
        : public NALUnitImpl
    {
    public:
        NALUnitSliceWithoutPartitioning();
        virtual ~NALUnitSliceWithoutPartitioning();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Coded slice extension
    class NALUnitSliceExtension
        : public NALUnitImpl
    {
    public:
        NALUnitSliceExtension();
        virtual ~NALUnitSliceExtension();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Unspecified type
    class NALUnitUnspecified
        : public NALUnitImpl
    {
    public:
        NALUnitUnspecified(unsigned char nut);
        virtual ~NALUnitUnspecified();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        unsigned char type() const;

        LOKI_DEFINE_VISITABLE()
    };

    /// @brief Reserved NAL Unit
    class NALUnitReserved
        : public NALUnitImpl
    {
    public:
        NALUnitReserved(unsigned char nut);
        virtual ~NALUnitReserved();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        unsigned char type() const;

        LOKI_DEFINE_VISITABLE()
    };


    /// @brief Unknown NAL Unit
    class NALUnitUnknown
        : public NALUnitImpl
    {
    public:
        NALUnitUnknown(unsigned char nut);
        virtual ~NALUnitUnknown();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        unsigned char type() const;

        LOKI_DEFINE_VISITABLE()
    };

};

#include "sps.h"
#include "pps.h"
#include "slice.h"


#endif