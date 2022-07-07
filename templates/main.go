package main

import (
	_ "embed"
	"flag"
	"fmt"
	"log"
	"os"
	"path"
	"strconv"
	"text/template"
)

//go:embed templ/clion_cmake_config_template.xml
var CLION_CMAKE_PROFILES string

//go:embed templ/clion_config_template.xml
var CLION_RUN_CONFIGS string

//go:embed templ/cmake_cmakelists_template.txt
var CMAKELISTS_TEMPLATE string

//go:embed templ/cpp_base_template.cpp
var CPP_TEMPLATE string

type ProjectConfig struct {
	Name         string
	ProblemCount int
	ProbArray    []string
}

func main() {
	conf := parseArgs()
	fmt.Printf("Creating project for: '%s' (%d problems)\n",
		conf.Name, conf.ProblemCount)

	buildDirectories(conf)
	writeProblemFiles(conf)
	writeClionConfigs(conf)

	fmt.Println("Finished.")
}

func parseArgs() *ProjectConfig {
	flag.Parse()
	args := flag.Args()
	if len(args) < 2 {
		log.Fatal("missing args: [PROJECT_NAME] [# PROBLEMS]")
	}
	pCount, err := strconv.ParseInt(args[1], 10, 32)
	if err != nil {
		log.Fatalf("could not parse # problems: %s", err)
	}
	conf := &ProjectConfig{
		Name:         args[0],
		ProblemCount: int(pCount),
	}
	for p := 1; p < conf.ProblemCount+1; p++ {
		conf.ProbArray = append(conf.ProbArray, fmt.Sprintf("%02d", p))
	}
	return conf
}

func buildDirectories(conf *ProjectConfig) {
	err := os.MkdirAll(
		path.Join(conf.Name, ".idea", "runConfigurations"),
		os.ModePerm,
	)
	if err != nil {
		log.Fatalf("could not build directory: %s", err)
	}
}

func writeProblemFiles(conf *ProjectConfig) {
	var err error
	var fileName string
	var f *os.File
	for p := 1; p < conf.ProblemCount+1; p++ {
		// empty #.stdin.txt
		fileName = fmt.Sprintf("problem_%02d.stdin.txt", p)
		f, err = os.Create(path.Join(conf.Name, fileName))

		if err != nil {
			log.Fatalf("could not create stdin file %02d: %s", p, err)
		}
		_ = f.Close()

		// fill #.cpp
		fileName = fmt.Sprintf("problem_%02d.cpp", p)
		f, err = os.OpenFile(path.Join(conf.Name, fileName),
			os.O_CREATE|os.O_WRONLY, os.ModePerm)
		if err != nil {
			log.Fatalf("could not create cpp file %02d: %s", p, err)
		}
		_, err = f.WriteString(CPP_TEMPLATE)
		if err != nil {
			log.Fatalf("could not write cpp template %02d: %s", p, err)
		}
		_ = f.Close()
	}
}

func writeClionConfigs(conf *ProjectConfig) {
	var err error
	var fileName string
	var f *os.File

	fileName = path.Join(conf.Name, ".idea", ".name")
	f, err = os.OpenFile(fileName, os.O_CREATE|os.O_WRONLY, os.ModePerm)
	if err != nil {
		log.Fatalf("could not create .name file: %s", err)
	}
	_, err = f.WriteString(conf.Name)
	if err != nil {
		log.Fatalf("could not write .name file: %s", err)
	}

	fillTemplateToFile(
		"cmake lists",
		path.Join(conf.Name, "CMakeLists.txt"),
		CMAKELISTS_TEMPLATE,
		conf,
	)

	fillTemplateToFile(
		"idea cmake config",
		path.Join(conf.Name, ".idea", "cmake.xml"),
		CLION_CMAKE_PROFILES,
		conf,
	)

	fillTemplateToFile(
		"idea cmake run configs",
		path.Join(conf.Name, ".idea", "cmake.xml"),
		CLION_CMAKE_PROFILES,
		conf,
	)

	t := template.Must(
		template.New("clion problem runconf").Parse(CLION_RUN_CONFIGS))
	for p := 1; p < conf.ProblemCount+1; p++ {
		fileName = path.Join(conf.Name, ".idea", "runConfigurations",
			fmt.Sprintf("problem_%02d.xml", p))
		f, err = os.OpenFile(fileName, os.O_CREATE|os.O_WRONLY, os.ModePerm)
		if err != nil {
			log.Fatalf("could not create runconf file %02d: %s", p, err)
		}
		err = t.Execute(f, map[string]interface{}{
			"ProblemID": fmt.Sprintf("%02d", p),
			"Name":      conf.Name,
		})
		if err != nil {
			log.Fatalf("could not write to runconf file %02d: %s", p, err)
		}
		_ = f.Close()
	}
}

func fillTemplateToFile(name, path, templ string, conf *ProjectConfig) {
	t := template.Must(template.New(name).Parse(templ))
	f, err := os.OpenFile(path, os.O_CREATE|os.O_WRONLY, os.ModePerm)
	if err != nil {
		log.Fatalf("could not create file for template '%s': %s", name, err)
	}
	err = t.Execute(f, conf)
	if err != nil {
		log.Fatalf("could not parse template '%s': %s", name, err)
	}
	_ = f.Close()
}
