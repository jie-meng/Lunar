-- Generate cmd
local pydoc_gen_cmd = 'pydoc'
if util.strContains(util.platformInfo(), 'windows', false) then
    pydoc_gen_cmd = 'python -m pydoc'
end

print("Please input target:")
local target = io.read()
print(target)
print()

if string.len(util.strTrim(target)) == 0 then
    print('Error: Input nothing')
    os.exit(0)
end

os.execute(pydoc_gen_cmd .. ' ' .. target)
