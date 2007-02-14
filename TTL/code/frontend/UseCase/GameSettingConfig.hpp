#ifndef _INCLUDED_GAME_SETTING_CONFIG_
#define _INCLUDED_GAME_SETTING_CONFIG_

#include "StringList.hpp"
#include "ValueList.hpp"

const int ANY = -1;

/// Base class for parameters that can be passed to the Multiplayer Game Settings dialog
class ParamType
{
    public:
        /** \name Constructor */
        //\{
        explicit ParamType(const char *pName) { strncpy(_name, pName, 50); }
        //\}

        /** \name Destructor */
        //\{
        virtual ~ParamType() { }
        //\}

        /** \name Accessors */
        //\{
        virtual const char *getStringValue() = 0;
        virtual int getIntegerValue() = 0;
        const char *getName() const { return _name; }
        virtual const char *getDescription() = 0;
        //\}

        /** \name Mutators */
        //\{
        virtual void nextValue() = 0;
        virtual void prevValue() = 0;
        //\}

    private:
        /** \name Constructor */
        //\{
        ParamType() { }  // Disallow default constructor
        //\}

        char _name[50];         /// < Name of this setting
};

/// Derived class for integer parameters that can be passed to the Multiplayer Game Settings dialog
class IntegerType : public ParamType
{
    public:
        /** \name Constructor */
        //\{
        explicit IntegerType(const char *pName, int increment = 1, int max = 1, int min = 0, int value = 0);
        //\}

        /** \name Accessors */
        //\{
        /* virtual */ const char *getStringValue();
        /* virtual */ int getIntegerValue();
		/* virtual */ const char *getDescription() { return ""; }
        //\}

        /** \name Mutators */
        //\{
        void setValue(int value);
        void setMin(int min);
        void setMax(int max);
        /* virtual */ void nextValue();
        /* virtual */ void prevValue();
        //\}

    private:
        /** \name Constructor */
        //\{
        IntegerType();  // Disallow default constructor
        //\}

        int _value;
        int _min;
        int _max;
        int _increment;
        char _stringValue[20];
};

/// Derived class for string parameters that can be passed to the Multiplayer Game Settings dialog
class StringType : public ParamType
{
    public:
        /** \name Constructor */
        //\{
        explicit StringType(const char *pName);
        //\}

        /** \name Accessors */
        //\{
        /* virtual */ const char *getStringValue();
        /* virtual */ int getIntegerValue();
		/* virtual */ const char *getDescription();
        //\}

        /** \name Mutators */
        //\{
        /* virtual */ void nextValue();
        /* virtual */ void prevValue();
        void addStringValue(const char *pValue, const char *pDescription = "");
        int getNumValues();
		int getIndexForValue( const char* value );
        //\}

    private:
        /** \name Constructor */
        //\{
        StringType();  // Disallow default constructor
        //\}

        StringList _values;
		StringList _descriptions;
};

/// Derived class for string and integer parameters that can be passed to the Multiplayer Game Settings dialog
class EnumType : public ParamType
{
    public:
        /** \name Constructor */
        //\{
        explicit EnumType(const char *pName);
        //\}

        /** \name Accessors */
        //\{
        /* virtual */ const char *getStringValue();
        /* virtual */ int getIntegerValue();
		/* virtual */ const char *getDescription();
        //\}

        /** \name Mutators */
        //\{
        /* virtual */ void nextValue();
        /* virtual */ void prevValue();
        void addValue(int integer, const char *pString, const char *pDescription = "");
        //\}

    private:
        /** \name Constructor */
        //\{
        EnumType();  // Disallow default constructor
        //\}

        typedef char charString[50];

        ValueList _values;
		StringList _descriptions;
};

#endif // _INCLUDED_GAME_SETTING_CONFIG_
