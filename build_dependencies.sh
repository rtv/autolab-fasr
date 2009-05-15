#!/bin/bash

PROJECT_DIR=`pwd`
DEPENDENS_SRC_DIR=$PROJECT_DIR/local/src
DEPENDENS_INSTALL_DIR=$PROJECT_DIR/local

PKG_CONFIG_PATH_ORG=$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$DEPENDENS_INSTALL_DIR/lib/pkgconfig

echo changing PKG_CONFIG_PATH to $PKG_CONFIG_PATH

echo creating install directory $DEPENDENCE_SRC_DIR
mkdir -p ${DEPENDENS_SRC_DIR}

######################################
# get ND
echo Fetching autolab-nd
cd ${DEPENDENS_SRC_DIR}
git clone git@github.com:rtv/autolab-nd.git
cd ${DEPENDENS_SRC_DIR}/autolab-nd

echo Reverting autolab-nd to the correct commit version
#git reset --hard a80def672b1cf887ba797bd5f43b3abd8ae600af

######################################
# get WP
echo Fetching autolab-wp
cd ${DEPENDENS_SRC_DIR}
git clone git@github.com:rtv/autolab-wp.git
cd ${DEPENDENS_SRC_DIR}/autolab-wp

echo Reverting autolab-wp to the correct commit version
#git reset --hard 14cf4e2c570ac5ff15bd487f67357583505c51b8

######################################
# get RAPI
echo Fetching autolab-rapi
cd ${DEPENDENS_SRC_DIR}
git clone git@github.com:jwawerla/autolab-rapi.git
cd ${DEPENDENS_SRC_DIR}/autolab-rapi

echo Reverting autolab-rapi to the correct commit version
#git reset --hard 14cf4e2c570ac5ff15bd487f67357583505c51b8




######################################
# Install rapi
cd ${DEPENDENS_SRC_DIR}/autolab-rapi
cmake -DCMAKE_INSTALL_PREFIX=${DEPENDENS_INSTALL_DIR}
make install

######################################
# Install nd
cd ${DEPENDENS_SRC_DIR}/autolab-nd
cmake -DCMAKE_INSTALL_PREFIX=${DEPENDENS_INSTALL_DIR} -DPKG_CONFIG_PATH=$DEPENDENS_INSTALL_DIR/lib/pkconfig
make install

######################################
# Install wp
cd ${DEPENDENS_SRC_DIR}/autolab-wp
cmake -DCMAKE_INSTALL_PREFIX=${DEPENDENS_INSTALL_DIR}
make install


######################################
# Finishing up

# restore PKG_CONFIG_PATH
#export PKG_CONFIG_PATH=$PKG_CONFIG_PATH_ORG

# change back to project dir and we are done
cd $PROJECT_DIR

