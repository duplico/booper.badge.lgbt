import sys
import os
import click

# In the following, FA is the badge ID, and 0E is the frequency.
INFOA_TXT = """@1800
FA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 01 00 0E 00 00"""
# TODO: needs a `q` by itself on the last line

@click.group()
def program_badge():
    pass

@click.command()
@click.option('-i', '--source-txt', default='code_and_cinit.txt', type=click.File())
@click.argument('id', type=int)
def flash_badge(id, source_txt):
    if id == 250:
        click.echo("WARNING:\tFlashing this badge using unassigned ID")
    if id > 100:
        click.echo("WARNING:\tFlashing this badge with ID over 100")
    click.echo("INFO:\tAttempting to flash badge %03d" % id)
    # Erase?
    # Put the INFOA on
    # Put the code on

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
