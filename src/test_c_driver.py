###############################################################################

#                 This program was produced by Trevor Thomas.                 #
#               Kick ass, go to space, represent the human race.              #

###############################################################################

'''
TEST C DRIVER
=============

PURPOSE         :
-------
Testing file for high level "driver" verification.

FIRST PRODUCED  :   2025/05/01 by Trevor Thomas

'''

###############################################################################
###                                 IMPORTS                                 ###
###############################################################################

import pytest

from pathlib import Path
from pytest import fixture
from subprocess import call, check_output

###############################################################################
###                                CONSTANTS                                ###
###############################################################################

PARENT_DIR:Path = Path(__file__).parent

###############################################################################
###                                GLOBALS                                  ###
###############################################################################



###############################################################################
###                            GLOBAL FUNCTIONS                             ###
###############################################################################

@fixture(scope='module',
         autouse=True)
def recompile_executable() -> None:
    call('gcc shape_cipher.c shape_cipher_driver.c'.split(),
         cwd=PARENT_DIR)

@pytest.mark.parametrize(argnames=('key', 'msg'),
                         argvalues=((3, 'hello from afar'),
                                    (4, 'hello from afar'),
                                    (5, 'scoobydoo')))
def test_driver_nominal(key:int,
                        msg:str) -> None:
    '''
    Tests the C driver program for various keys and messages.

    Parameters
    ----------
    key : int
        Integer encoding key.
    msg : str
        The message to encode decode.
    '''
    cmd:list[str] = f'{PARENT_DIR / 'a.out'} encrypt {key}'.split()
    cmd += [f'"{msg}"']
    encrypted:str = check_output(cmd, cwd=PARENT_DIR).decode().strip()
    cmd:list[str] = f'{PARENT_DIR / 'a.out'} decrypt {key}'.split()
    cmd += [encrypted]
    decrypted:str = check_output(cmd, cwd=PARENT_DIR).decode().strip()
    assert msg.upper().replace(' ', '') in decrypted
    
###############################################################################
###                                 CLASSES                                 ###
###############################################################################



###############################################################################
###                                  MAIN                                   ###
###############################################################################



###############################################################################
###                                  NOTES                                  ###
###############################################################################
