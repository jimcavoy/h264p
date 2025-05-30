#ifndef VLC_H
#define VLC_H


namespace ThetaStream
{
    class Bitstream;

    class SyntaxElement
    {
    public:
        SyntaxElement();
        ~SyntaxElement();

    public:
        int           type;                  //!< type of syntax element for data part.
        int           value1;                //!< numerical value of syntax element
        int           value2;                //!< for blocked symbols, e.g. run/level
        int           len;                   //!< length of code
        int           inf;                   //!< info part of CAVLC code
        unsigned int  bitpattern;            //!< CAVLC bitpattern
        int           context;               //!< CABAC context
        int           k;                     //!< CABAC context for coeff_count,uv

        //! for mapping of CAVLC to syntaxElement
        void  (*mapping)(int len, int info, int* value1, int* value2);
    };


    int se_v(ThetaStream::Bitstream* bitstream);
    int ue_v(ThetaStream::Bitstream* bitstream);
    bool u_1(ThetaStream::Bitstream* bitstream);
    int u_v(int LenInBits, ThetaStream::Bitstream* bitstream);
    int i_v(int LenInBits, ThetaStream::Bitstream* bitstream);

    void linfo_ue(int len, int info, int* value1, int* dummy);
    void linfo_se(int len, int info, int* value1, int* dummy);

    int readSyntaxElement_VLC(SyntaxElement* sym, ThetaStream::Bitstream* currStream);
    int readSyntaxElement_FLC(SyntaxElement* sym, ThetaStream::Bitstream* currStream);

}
#endif