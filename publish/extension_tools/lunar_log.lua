local ds = net.udpCreate(1)

local port = 9966
if net.udpBind(ds, "", port) then
    print(string.format("Recv log on port %d start:\n", port))
    while true do
        local len = net.udpRecvFrom(ds)
        if len > 0 then
            print(memory.tostring(net.udpGetRecvBuf(ds)))
        else
            break
        end
    end    
end

net.udpDestroy(ds)

print(string.format("Recv log on port %d end.", port))