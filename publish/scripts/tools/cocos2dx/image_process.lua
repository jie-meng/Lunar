local ImageProcess = {}

function ImageProcess.appendImages(output, vertical, ...)
    local args = {...}
    local s = ""
    for _, v in pairs(args) do
        s = s .. " " .. v
    end
    
    local append_mode = "+"
    if (vertical) then
        append_mode = "-"
    end
        
    os.execute("convert" .. s .. " " .. append_mode .. "append " .. output)
end

function ImageProcess.appendAllImagesInPath(output, vertical, path)
    local t = file.findFilesInDir(path)
    table.sort(t)
    for k, v in pairs(t) do
        print("append " .. k .. " " .. v)
    end
    
    local s = ""
    for _, v in pairs(t) do
        s  = s .. " " .. v
    end
    
    ImageProcess.appendImages(output, vertical, s)
end

--[[
ImageProcess.appendAllImagesInPath("shit.png",  false, "./icons")
print("ok")
--]]

return ImageProcess