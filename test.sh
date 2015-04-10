#!/bin/sh

echo "\\033[1m\\033[37m\\033[42m***************** CMAKE - DEBUG *****************\\033[0m"
mkdir -p build_debug
cd build_debug
if cmake -G "Eclipse CDT4 - Ninja" -D CMAKE_BUILD_TYPE=Debug ../src
then
  echo "\\033[1m\\033[37m\\033[42m***************** BUILD - DEBUG *****************\\033[0m"
  if ninja -v
  then
    echo "\\033[1m\\033[37m\\033[42m***************** TEST - DEBUG *****************\\033[0m"
    if ninja -v test
    then
      cd ..


echo "\\033[1m\\033[37m\\033[42m***************** CMAKE - RELEASE *****************\\033[0m"
mkdir -p build_release
cd build_release
if cmake -G "Ninja" -D CMAKE_BUILD_TYPE=Release ../src
then
  echo "\\033[1m\\033[37m\\033[42m***************** BUILD - RELEASE *****************\\033[0m"
  if ninja -v
  then
    echo "\\033[1m\\033[37m\\033[42m***************** TEST - RELEASE *****************\\033[0m"
    if ninja -v test
    then
      cd ..
      echo "\\033[1m\\033[37m\\033[42m*** DEBUG OUTPUT ***\\033[0m"
      cat build_debug/Testing/Temporary/LastTest.log
      echo "\\033[1m\\033[37m\\033[42m*** RELEASE OUTPUT ***\\033[0m"
      cat build_release/Testing/Temporary/LastTest.log
      echo "\\033[1m\\033[37m\\033[42m***************** ALL IS WELL THAT ENDS WELL *****************\\033[0m"
    else
      echo "\\033[1m\\033[37m\\033[41m***************** TEST - RELEASE *****************   F A I L   ! ! !\\033[0m"
      cat Testing/Temporary/LastTest.log
    fi
  else
    echo "\\033[1m\\033[37m\\033[41m***************** BUILD - RELEASE *****************   F A I L   ! ! !\\033[0m"
  fi
else
  echo "\\033[1m\\033[37m\\033[41m***************** CMAKE - RELEASE *****************   F A I L   ! ! !\\033[0m"
fi


    else
      echo "\\033[1m\\033[37m\\033[41m***************** TEST - DEBUG *****************   F A I L   ! ! !\\033[0m"
      cat Testing/Temporary/LastTest.log
    fi
  else
    echo "\\033[1m\\033[37m\\033[41m***************** BUILD - DEBUG *****************   F A I L   ! ! !\\033[0m"
  fi
else
  echo "\\033[1m\\033[37m\\033[41m***************** CMAKE - DEBUG *****************   F A I L   ! ! !\\033[0m"
fi

