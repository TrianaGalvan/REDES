-- protocolo de Triana
-- declarando mi protocolo
triana_proto = Proto("triana","Protocolo de Triana")
-- creando una funcion para la disecci�n.
function triana_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "Triana"
    
	local subtree = tree:add(triana_proto,buffer(),"Datos del Protocolo Triana")
    subtree:add(buffer(0,1), "Tid: " .. buffer(0,1):string())
    subtree:add(buffer(1,2), "Opcode: " .. buffer(1,2))	
    subtree:add(buffer(3,2), "Block Number: " .. buffer(3,4))	
	-- Obtener el tama�o reportado por wirechark
	--local pktlen = buffer:reported_length_remaining()
	
	--subtree:add(buffer:range(2,pktlen-2),"Datos: " .. string.format("%s",buffer:range(2,pktlen-2)))
	subtree:add(buffer:range(3),"Datos: " .. buffer:range(3):string())
    
end
-- load the udp.port table
udp_table = DissectorTable.get("ethertype")
--
-- register our protocol to handle udp port 7777
udp_table:add(0xe000,triana_proto)