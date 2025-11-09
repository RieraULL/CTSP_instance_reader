#!/bin/csh
    alias acalc '       awk "BEGIN{ print \!* }" '

    set fecha  = `date "+%y%m%d%H%M%S"`
	set fechab = `date "+%d/%m/%Y %H:%M:%S"`

	set basePath      = ./
	set baseExePath   = ${basePath}/build/
   	set programPath   = ${baseExePath}main/
	set solverExe     = ${programPath}ctsp_reader

    set inputPath     = ${basePath}input/SubramanyamGounaris/
	set outputPath    = ${basePath}output/


	set instanceList  = ( burma14 ulysses16 br17 gr17 gr21 ulysses22 gr24 fri26 bayg29 bays29 ftv33 ftv35 ftv38 dantzig42 swiss42 p43 ftv44 ftv47 att48 gr48 hk48 ry48p eil51 berlin52 ft53 ftv55 brazil58 ftv64 ft70 ftv70 st70 eil76 pr76 gr96 rat99 kroA100 kroB100 kroC100 kroD100 kroE100 rd100 eil101  )
	
	echo Starting: ${fechab}
	echo " "

	

	set outputPath    = ${outputPath}CTSP${fecha}/

	mkdir -p ${outputPath}

	foreach familyName ( ${instanceList} )

		echo " "

		echo "** Processing family:  " ${familyName}

		set familyPath = ${outputPath}${familyName}/

		mkdir -p ${familyPath}

		set outputLog     = ${familyPath}log/

		mkdir -p ${outputLog}

		echo " "

		ls ${inputPath}${familyName}*.contsp

		foreach targetFile (${inputPath}${familyName}*.contsp)

			set targetName   = $targetFile:t:r

			echo "== Processing instance:" ${targetName}

			set instanName    = $targetName:t:r
			set logFileName   = ${outputLog}${targetName}.log


			echo " "
			echo "      Log File: " ${logFileName}
			echo " "

			${solverExe} ${targetFile} ${logFileName}

			echo "== End of instance: " ${targetName}
			echo " "	

		end

	end


	set fechae = `date "+%d/%m/%Y %H:%M:%S"`

	echo " "
	echo Ending: ${fechae} 	


