: # this should work in both linux or windows cmd using the GOTO technique

:<<"::CMDLITERAL"
@ECHO OFF
GOTO :CMDSCRIPT
::CMDLITERAL

echo "this part should only run in linux"
echo "if ur using fish this aint gonna work bud, but you can run this using bass"
python --version # Fix if its broken later
if [$? -eq 0 ]; then
    echo "python installed"
else
    echo "python not detected. Please go to the python website and install it."
    exit $?
fi

pip --version 
if [$? -eq 0 ]; then
    echo "pip installed"
else
    echo "pip not detected. "
    curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
    python get-pip.py
fi

cd env
if [$? -eq 0 ]; then
    echo "env file found. Running virtual environment..."
else
    echo "env file not found. Installing env..."
    pip install -m venv env
fi

sh Scripts/activate
cd ..

echo "virtual environment activated, checking requirements"

pip install -r requirements.txt
if [$? -eq 0 ]; then
    echo "requirements updated. Running python script..."
else
    echo "requirements file not found. Please install it to this directory"
    exit $?
fi

python KinesiologyPyConnection.py --project-id kinetest-337921 --session-id 83b1d725-3e6b-a062-2cc2-5de9f9788f6f
if [$? -eq 0 ]; then
    echo "success"
else
    echo "python script not found. Please install the python script to the directory"
fi

exit $?

:CMDSCRIPT
ECHO this should only run in windows
:: checking python installation
python --version 2>NUL
if ERRORLEVEL 1 GOTO errNoPy

GOTO :pyIn

:errNoPy
ECHO python is not installed on your machine. Please go to the python website to install the latest version.
start "" "https://www.python.org/downloads/windows/"
GOTO :EOF

:pyIn
ECHO python detected. Checking pip...
:: check if pip is installed, and if not install it
pip --version 2>NUL
if ERRORLEVEL 1 GOTO errNoPip

GOTO :goodPip

:errNoPip
ECHO pip is not installed on your machine. Attempting to install now...
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python get-pip.py
ECHO Succeessfully installed pip

:goodPip
ECHO pip detected. Starting virtual environment...
:: check if env file is in the right spot
if exist env (
    ECHO env file detected. Starting virtual environment...
) else (
    ECHO env file not detected. Attempting to install now...
    pip install -m venv env
)

:: activate virtual environment
call env\Scripts\activate.bat
ECHO virtual environment activated. Checking requirements file...

:: check if the requirements file is here
if exist requirements.txt (
    ECHO requirements file found. Checking if dependencies need updating...
    pip install -r requirements.txt
) else (
    ECHO requirements file not found. Please put requirements.txt in the correct folder.
    GOTO :EOF
)

:: check if python script exists
if exist KinesiologyPyConnection.py (
    ECHO python script found. Running script...
    python KinesiologyPyConnection.py --project-id kinetest-337921 --session-id 83b1d725-3e6b-a062-2cc2-5de9f9788f6f
) else (
    ECHO python script not found. Please install it in the correct directory
    GOTO :EOF
)