#pragma once
#include <loki/Visitor.h>
#include <h264p/seiimpl.h>

#include <memory.h>
#include <vector>

namespace boost
{
    namespace json
    {
        class object;
    }
}

namespace avc2json
{
    /// @brief This class represents a Modified Digital Video Pack Metadata (MDPM) element
    /// of the unregistered user data with UUID 17ee8c60-f84d-11d9-8cd6-0800200c9a66
    /// in the H.264 Supplemental Enhancement Information (SEI).
    class MDPMElement
    {
    public:
        MDPMElement(char t = (char)0xFF, char* value = NULL)
            :tag_(t) {
            memcpy(value_, value, 4);
        }
        ~MDPMElement() {}
        MDPMElement(const MDPMElement& orig)
        {
            tag_ = orig.tag();
            memcpy(value_, orig.value(), 4);
        }

        MDPMElement& operator=(const MDPMElement& rhs)
        {
            tag_ = rhs.tag();
            memcpy(value_, rhs.value(), 4);
            return *this;
        }

        char tag() const { return tag_; }
        const char* value() const { return value_; }

    private:
        char tag_;
        char value_[4];
    };


    /// @brief Modified Digital Video Pack Metadata (MDPM) parser. 
    class MDPMParser :
        public Loki::BaseVisitor,
        public Loki::Visitor<ThetaStream::SEIUserDataUnreg>
    {
    public:
        typedef std::vector<MDPMElement> ElementCollection;
        typedef ElementCollection::iterator iterator;
    public:
        MDPMParser(boost::json::object& nalu);
        ~MDPMParser(void);

        void Visit(ThetaStream::SEIUserDataUnreg& sei);

    private:
        ElementCollection elements_;
        boost::json::object& nalu_;
    };

}
