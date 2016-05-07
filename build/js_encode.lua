#! /usr/local/bin/lua
local io=require "io"
local os=require "os"
local string=require "string"

if (#arg ~= 2) then 
	print("wrong number of argument:"..#arg.." needs 2")
	print("example: ./json_encode ./source_file.json ./encoded_file.json")
	os.exit()
end


local file = io.open(arg[1], "r")
local str = file:read("*a")
file:close()

local i,j= str:find("/%*%%%d*%w*%*/") 
if (i and j) then
	--(str:sub(i,j):find("%w*%d"))
end

function findByEnclosure(str,l,r)
	local i,j = str:find(l..".-"..r)
	if i and j then
		--print("i : "..i.." j : "..j)
		local substr = str:sub(i,j)
		local m,n = substr:find(l)
		local o,p = substr:find(r)
		local substr = substr:sub(n+1,o-1)
		local r,s = substr:find("[%w%$_].*[%w%$_]")
		local substr = substr:sub(r,s)
		return i,j,substr
	end
	return nil
end

function checkBalance(str,ll,lr, rl,rr)
	local balance = 0
	local case = 0
	local locStr = str	
	while (true) do
		local i,j,pattern= findByEnclosure(locStr,ll,lr)
		if i and j then
			balance = balance + 1
			local k,l = locStr:find(rl..pattern..rr)
			if k and l then
				balance = balance - 1
				locStr=locStr:sub(l)
				case = case + 1
			else
				print("file not balanced,expecting "..pattern)
				break
			end
		else
			break
		end
	end
	if (balance==0 and case>0) then
		return true
	else
		if (case==0) then
			print("no data found")
		end
		return false
	end
end

function extractContent(str,ll,lr,rl,rr)
	local i,j,pattern= findByEnclosure(str,ll,lr)
	if i and j then
		local k,l=str:find(rl..pattern..rr)	
		if (k == nil) then
			print(i.."  "..j)		
		return nil		
		end
		local subStr = str:sub(j+1, k-1)
		local m,n = subStr:find("%S.*%S")
		local content = subStr:sub(m,n)
		return pattern, content, l+1
	else
		return nil
	end
end

function stripEnclosure(str, l, r)
	local i,j,enclosure = str:find(l..".-"..r)
	local rtn = str	
	if i and j then
		local comment = rtn:sub(i,j)
		--print(comment)
		comment = comment:gsub("[%^%$%(%)%%%.%[%]%*%+%-%?%)]","%%%1")
		rtn = rtn:gsub(comment, "")
	else 
		--print('nothing')
	end	

	return rtn
end

function stripAllEnclosure(str, l, r)
	local localStr = str
	local tempStr	= str

	localStr = stripEnclosure(localStr, l, r)
	
	while (localStr ~= tempStr) do
		localStr = tempStr
		tempStr = stripEnclosure(tempStr, l, r)
	end
	return localStr
end
---[[
local l="/*%{"
local r="}%*/"
local ll = "/%*%%"
local lr = "%*/"
local rl = "/%*"
local rr = "%%%*/"
--]]
--[[
local l="/*%{"
local r="}%*/"
local ll = "/%*@"
local lr = "%*/"
local rl = "/%*"
local rr = '*/'
--]]
function extractSingleEnclosure(str, l, r)
	local i,j,pattern= findByEnclosure(str,l,r)
	--print(pattern)

	if (pattern) then
		local patternTable={}
		for w in pattern:gmatch("[^,]+") do
			table.insert(patternTable,1,w)      
			print(w)
  	end
		return i,j,patternTable
	else
		return nil
	end
end



res = checkBalance(str, ll, lr, rl, rr)
if res==false then
	--os.exit()
	print("warning: structure is not balanced")
end

local rtn= {nil}
function extractPath(str, ll, lr, rl, rr,root)
	local string = str
	local key,value,pointer = extractContent(string, ll, lr, rl, rr)
	local rtn = root or {}
	local tablePtr = rtn
	local tablePtra, wa
	while (value) do
		tablePtr = rtn
		for w in key:gmatch("[^,]+") do
			if (not tablePtr[w]) then
				tablePtr[w] = {}			
			end
			tablePtra = tablePtr
			wa = w 
			tablePtr = tablePtr[w]
		end
		--value=value:gsub("[%^%$%(%)%%%.%[%]%*%+%-%?%)]","%%%1")
		local tempRtn = extractPath(value, ll, lr, rl, rr,tablePtr)
		
		if (not next(tempRtn)) then
			tablePtra[wa] = value		
		else
		
		end 
	
		string = string:sub(pointer)
		key,value,pointer = extractContent(string, ll, lr, rl, rr)
	end 
	return rtn
end

local wow = extractPath(str, ll, lr, rl, rr,root)

function printTable(table)
	local str=""
	for k,v in pairs(table) do 
		
		if type(v) == "table" then
			str=str.."\""..k.."\":{"..printTable(v).."}"
		else
			str=str.."\""..k.."\":"..v
		end
		if (next(table,k)) then
			str=str..","		
		end
		
	end
	return str
end
---[[
function elysium(table)
	local str=""
	for k,v in pairs(table) do 
		
		if type(v) == "table" then
			str=str.."var "..k.." = {"..printTable(v).."}"
		else
			str=str.."var "..k.." ="..v
		end
		if (next(table,k)) then
			str=str..";"		
		end
		
	end
	return str
end
--]]
--outString = "{"..printTable(wow).."}"
--outString = "var $k = "..outString..";"
outString = elysium(wow)
--outString = outString:gsub("[\n\t]","")
outString = stripAllEnclosure(outString,"/%*","%*/")
outString = stripAllEnclosure(outString,"//","\n")
local file = io.open(arg[2], "w")
file:write(outString)
file:close()





