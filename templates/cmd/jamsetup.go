package main

import (
	_ "embed"
	"flag"
	"fmt"
	cj "github.com/waltage/codejam/templates"
	"log"
	"strconv"
)

func main() {
	project := parseArgs()
	fmt.Printf("Creating project for: '%s' (%d problems)\n",
		project.Name, project.ProblemCount)

	project.BuildDirectories()
	project.WriteProblemFiles()
	project.WriteClionConfigs()

	fmt.Println("Finished.")
}

func parseArgs() *cj.ProjectConfig {
	flag.Parse()
	args := flag.Args()
	if len(args) < 2 {
		log.Fatal("missing args: [PROJECT_NAME] [# PROBLEMS]")
	}
	pCount, err := strconv.ParseInt(args[1], 10, 32)
	if err != nil {
		log.Fatalf("could not parse # problems: %s", err)
	}
	return cj.NewProject(args[0], int(pCount))
}
