local post_time = os.date('%Y-%m-%d')
local content_time = os.date('%Y-%m-%d %I:%M:%S +0800')

print('Please input your title:')
local title = io.read()
print(title)
local post_name = post_time .. '-' .. string.lower(util.strReplaceAll(title, ' ', '-')) .. '.md'

print('Please input your category:')
local category = io.read()
print(category)

local path, _ = util.splitPathname(util.appPath())
local post_content = util.readTextFile(path .. '/assets/templates/gh-pages/post.md')
post_content = util.strReplaceAll(post_content, '{$TITLE$}', title)
post_content = util.strReplaceAll(post_content, '{$DATE_TIME$}', content_time)
post_content = util.strReplaceAll(post_content, '{$CATEGORY$}', category)
util.writeTextFile(post_name, post_content)

print(post_name .. ' created.')
