#!/usr/bin/env bash
# Simple test harness to ensure that the browser still works.

#Colours
red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
reset=`tput sgr0`

#Messages
TEST_PASS="${green}● Test Passed${reset}"
TEST_FAIL="${red}● Test Failed${reset}"
PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0
#Check output of CLI has not changed.
echo "Normal Output test: "
if diff <(cat tests/normal_output) <(./browser http://example.com 2>&1) ; then
	echo -e "${TEST_PASS}"
	PASS_COUNT=$(( PASS_COUNT + 1 ))
else
	echo -e "${TEST_FAIL}"
	FAIL_COUNT=$(( FAIL_COUNT + 1 ))
fi
echo "Verbose Output test: "
if diff <(cat tests/verbose_output) <(./browser http://example.org -v 2>&1) ; then
	echo -e "${TEST_PASS}"
	PASS_COUNT=$(( PASS_COUNT + 1 ))
else
	echo -e "${TEST_FAIL}"
	FAIL_COUNT=$(( FAIL_COUNT + 1 ))
fi
# We expect these to coredump; do not remove -n
echo -n "404 Output test: "
if diff <(cat tests/404_output) <(./browser http://example.com/notFound.html 2>&1) ; then
	echo -e "${TEST_PASS}"
	PASS_COUNT=$(( PASS_COUNT + 1 ))
else
	echo -e "${TEST_FAIL}"
	FAIL_COUNT=$(( FAIL_COUNT + 1 ))
fi
# We expect these to coredump; do not remove -n
echo -n "Protocol Error Output test: "
if diff <(cat tests/protocolError_Output) <(./browser example.com 2>&1) ; then
	echo -e "${TEST_PASS}"
	PASS_COUNT=$(( PASS_COUNT + 1 ))
else
	echo -e "${TEST_FAIL}"
	FAIL_COUNT=$(( FAIL_COUNT + 1 ))
fi
echo "Summary:"
echo -e "${green}${PASS_COUNT} tests passed${reset}, ${red}${FAIL_COUNT} tests failed${reset}, ${yellow}${SKIP_COUNT} tests skipped${reset}"
