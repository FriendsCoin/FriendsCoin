#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-FRIC");
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", false));
    BOOST_CHECK(GetBoolArg("-FRIC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-FRICo"));
    BOOST_CHECK(!GetBoolArg("-FRICo", false));
    BOOST_CHECK(GetBoolArg("-FRICo", true));

    ResetArgs("-FRIC=0");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));

    ResetArgs("-FRIC=1");
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", false));
    BOOST_CHECK(GetBoolArg("-FRIC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noFRIC");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));

    ResetArgs("-noFRIC=1");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));

    ResetArgs("-FRIC -noFRIC");  // -FRIC should win
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", false));
    BOOST_CHECK(GetBoolArg("-FRIC", true));

    ResetArgs("-FRIC=1 -noFRIC=1");  // -FRIC should win
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", false));
    BOOST_CHECK(GetBoolArg("-FRIC", true));

    ResetArgs("-FRIC=0 -noFRIC=0");  // -FRIC should win
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--FRIC=1");
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", false));
    BOOST_CHECK(GetBoolArg("-FRIC", true));

    ResetArgs("--noFRIC=1");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", "eleven"), "eleven");

    ResetArgs("-FRIC -bar");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", "eleven"), "");

    ResetArgs("-FRIC=");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", "eleven"), "");

    ResetArgs("-FRIC=11");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", "eleven"), "11");

    ResetArgs("-FRIC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-FRIC", 0), 0);

    ResetArgs("-FRIC -bar");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-FRIC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-FRIC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--FRIC");
    BOOST_CHECK_EQUAL(GetBoolArg("-FRIC"), true);

    ResetArgs("--FRIC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-FRIC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noFRIC");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));

    ResetArgs("-noFRIC=1");
    BOOST_CHECK(!GetBoolArg("-FRIC"));
    BOOST_CHECK(!GetBoolArg("-FRIC", true));
    BOOST_CHECK(!GetBoolArg("-FRIC", false));

    ResetArgs("-noFRIC=0");
    BOOST_CHECK(GetBoolArg("-FRIC"));
    BOOST_CHECK(GetBoolArg("-FRIC", true));
    BOOST_CHECK(GetBoolArg("-FRIC", false));

    ResetArgs("-FRIC --noFRIC");
    BOOST_CHECK(GetBoolArg("-FRIC"));

    ResetArgs("-noFRIC -FRIC"); // FRIC always wins:
    BOOST_CHECK(GetBoolArg("-FRIC"));
}

BOOST_AUTO_TEST_SUITE_END()
