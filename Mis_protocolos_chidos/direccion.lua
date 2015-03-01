
-- protocolo de Triana
-- declarando mi protocolo
triana_proto = Proto("triana","Protocolo de Triana")
-- creando una funcion para la disección.
function triana_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "Triana"
    
	subtree = tree:add(triana_proto,buffer(),"Datos del Protocolo Triana")
	content = buffer(2,2):int()			

	if (content == 1) then do
		subtree:add(buffer(0,1), "Direccion origen: " .. buffer(0,1):string())	
    	subtree:add(buffer(1,1), "Direccion destino: " .. buffer(1,1):string())		
		subtree:add(buffer(2,2), "Opcode RRQ: " .. buffer(2,2))
		
		i = 5
		filename = ""
		while buffer(i,1) do 
			filename = filename .. buffer(i,1)
			i = i+1
		end
		subtree:add(buffer:range(5,i), "Nombre del archivo: ".. filename)
		
		j = i +1
		modo = ""
		while buffer(j,1) do 
			modo = modo .. buffer(j,1)
			j = j+1
		end
		subtree:add(buffer:range(i,j), "Modo: ".. modo)
	end

	elseif (content == 2) then do
		subtree:add(buffer(0,1), "Direccion origen: " .. buffer(0,1):string())	
    	subtree:add(buffer(1,1), "Direccion destino: " .. buffer(1,1):string())		
		subtree:add(buffer(2,2), "Opcode WRQ: " .. buffer(2,2))
		
		i = 4
		filename = ""
		while buffer(i,1) do
			filename = filename .. buffer(i,1):string()
			i = i+1
			if buffer(i,1):int() == 0 then break end  
		end

		subtree:add(buffer:range(4,i-4), "Nombre del archivo: ".. filename)
		
		j = i +1
		modo = ""
		while buffer(j,1) do 
			modo = modo .. buffer(j,1):string()
			j = j+1
			if buffer(j,1):int() == 0 then break end  
		end
		subtree:add(buffer:range(i+1,j-i), "Modo: ".. modo)
		

	end

	elseif (content == 3) then do
		subtree:add(buffer(0,1), "Direccion origen: " .. buffer(0,1):string())	
    		subtree:add(buffer(1,1), "Direccion destino: " .. buffer(1,1):string())		
		subtree:add(buffer(2,2), "Opcode DATA: " .. buffer(2,2))
		subtree:add(buffer(4,2), "Bloque: " .. buffer(4,2):int())
		tam  = buffer:len() - 8
		subtree:add(buffer(6,tam), "Datos: ".. buffer(6,tam):string())
		subtree:add(buffer(tam+6,2), "Checksum: ".. buffer(tam+6,2))
		end 

	elseif (content == 4) then do
		subtree:add(buffer(0,1), "Direccion origen: " .. buffer(0,1):string())	
    		subtree:add(buffer(1,1), "Direccion destino: " .. buffer(1,1):string())		
		subtree:add(buffer(2,2), "Opcode ACK: " .. buffer(2,2))
		subtree:add(buffer(4,2), "Bloque: " .. buffer(4,2):int())	
		end	

	end
	

	if (content == 5) then 
		subtree:add(buffer(0,1), "Direccion origen: " .. buffer(0,1):string())	
    		subtree:add(buffer(1,1), "Direccion destino: " .. buffer(1,1):string())		
		subtree:add(buffer(2,2), "Opcode ERROR: " .. buffer(2,2))
		subtree:add(buffer:range(5), "Datos: ".. buffer:range(5):string())	
	end 	

	
    
end
-- load the udp.port table
udp_table = DissectorTable.get("ethertype")
--
-- register our protocol to handle udp port 7777
udp_table:add(0xe000,triana_proto)