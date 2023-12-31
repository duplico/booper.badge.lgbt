import subprocess
import os
import click

# In the following, FA is the badge ID, and 0E is the frequency.
INFOA_TXT = """@1800
FA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 01 00 0E XX 00"""
# TODO: needs a `q` by itself on the last line

@click.group()
def program_badge():
    pass

def do_flash_infoa(id, freq=None, infoa_base=INFOA_TXT):
    id_hex = '%02X' % id
    my_infoa = infoa_base
    my_infoa = my_infoa.replace('FA', id_hex, 1)
    if freq is not None:
        freq_hex = '%02X' % freq
        my_infoa = my_infoa.replace('XX', '01', 1)
        my_infoa = my_infoa.replace('0E', freq_hex)
    else:
        my_infoa = my_infoa.replace('XX', '00')
    with open('.infoa.tmp.txt', 'w') as infoa:
        print(my_infoa, file=infoa)
        print('q', file=infoa)
    subprocess.run(
        [
            'msp430flasher',
            '-e',
            'ERASE_ALL',
            '-v',
            '-w',
            '.infoa.tmp.txt',
            '-i',
            'TIUSB',
            '-j',
            'fast'
        ]
    )

@click.command()
@click.argument('id', type=int)
@click.option('--freq', type=int, default=None)
def flash_infoa(id, freq):
    do_flash_infoa(id, freq=freq)

program_badge.add_command(flash_infoa)

def do_flash_program(source_txt):
    subprocess.run(
        [
            'msp430flasher',
            '-e',
            'ERASE_MAIN',
            '-v',
            '-w',
            str(source_txt),
            '-i',
            'TIUSB',
            '-j',
            'fast'
        ]
    )

@click.command()
@click.option('-i', '--source-txt', default='code_and_cinit.txt', type=click.Path(file_okay=True, dir_okay=False, exists=True, readable=True))
def flash_program(source_txt):
    do_flash_program(source_txt)

program_badge.add_command(flash_program)

@click.command()
@click.option('-i', '--source-txt', default='code_and_cinit.txt', type=click.Path(file_okay=True, dir_okay=False, exists=True, readable=True))
@click.argument('id', type=int)
@click.option('--freq', type=int, default=None)
def flash_badge(id, source_txt, freq):
    if id == 250:
        click.echo("WARNING:\tFlashing this badge using unassigned ID")
    if id > 100:
        click.echo("WARNING:\tFlashing this badge with ID over 100")
    click.echo("INFO:\tAttempting to flash badge %03d" % id)
    do_flash_infoa(id, freq=freq)
    do_flash_program(source_txt)

program_badge.add_command(flash_badge)

@click.command()
@click.argument('source_file', type=click.File())
@click.option('-o', '--dest-path', type=click.Path(file_okay=False, dir_okay=True, exists=True, writable=True), default='./')
def copy_txt(source_file, dest_path='./'):
    out_txt = ""
    infoa_txt = ""
    infoa_appending = False
    for line in source_file:
        if line.startswith('@'):
            if line.startswith('@1800'): # INFOA location
                infoa_appending = True
            else:
                infoa_appending = False # Any other memory address
        if infoa_appending:
            infoa_txt += line
        else:
            out_txt += line
    
    if infoa_txt.strip() != INFOA_TXT.strip():
        click.echo("ERROR:\t INFOA contents do not match expected. Please confirm values.")

    with open(os.path.join(dest_path, 'code_and_cinit.txt'), 'w') as out_file:
        out_file.write(out_txt)

program_badge.add_command(copy_txt)

if __name__ == '__main__':
    program_badge()
