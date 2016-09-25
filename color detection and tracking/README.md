# Empty Project for PolySync 2.x with C++ #

This is a basic template for creating applications using the PolySync 2.x C++ API.

To customize this template for your application, simply edit the new_project.sh file
and replace the values of NEW_CLASS, NEW_HG, and NEW_REPO with the name of your project
in the proper format. Once this is done, run: `./new_project.sh`.

Note: This repository will be tagged with version numbers which match the PolySync version
with which the project was compatible at the time of release. To use a previous version, simply
roll back to the previous tag using the git command:

    git checkout -b <branch_name> v<version>

Where <branch_name> is the name of the new branch you'll be creating (not super important)
and <version> is the version number you want to roll back to. An example would be:

    git checkout -b new_branch v2.0.5
